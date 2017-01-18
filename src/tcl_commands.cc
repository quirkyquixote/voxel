/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "tcl_commands.h"

#include "context.h"
#include "lighting.h"

static box3ll sel_bb = { 0, 0, 0, 0, 0, 0 };

int parse_item(const char *str, int *mat, int *obj)
{
	for (*mat = MAT_COUNT - 1; *mat >= 0; --*mat) {
		if (mat_names[*mat] && strncmp(str, mat_names[*mat], strlen(mat_names[*mat])) == 0)
			break;
	}
	if (*mat < 0) {
		log_warning("%s: not a valid material", str);
		return -1;
	}
	str += strlen(mat_names[*mat]);
	if (*str == 0) {
		*obj = 0;
		return 0;
	}
	if (*str != '_') {
		log_warning("expected '_' after %s", mat_names[*mat]);
		return -1;
	}
	++str;
	for (*obj = OBJ_COUNT - 1; *obj >= 0; --*obj) {
		if (obj_names[*obj] && strncmp(str, obj_names[*obj], strlen(obj_names[*obj])) == 0)
			break;
	}
	if (*obj < 0) {
		log_warning("%s: not a valid object", str);
		return -1;
	}
	return 0;
}

int parse_block(const char *str, int *mat, int *shape)
{
	if (strcmp(str, "air") == 0) {
		*shape = SHAPE_NONE;
		return 0;
	}
	for (*mat = MAT_COUNT - 1; *mat >= 0; --*mat) {
		if (mat_names[*mat] && strncmp(str, mat_names[*mat], strlen(mat_names[*mat])) == 0)
			break;
	}
	if (*mat < 0) {
		log_warning("%s: not a valid material", str);
		return -1;
	}
	str += strlen(mat_names[*mat]);
	if (*str == 0) {
		*shape = SHAPE_BLOCK_DN;
		return 0;
	}
	if (*str != '_') {
		log_warning("expected '_' after %s", mat_names[*mat]);
		return -1;
	}
	++str;
	for (*shape = SHAPE_COUNT - 1; *shape >= 0; --*shape) {
		if (shape_names[*shape] && strncmp(str, shape_names[*shape], strlen(shape_names[*shape])) == 0)
			break;
	}
	if (*shape < 0) {
		log_warning("%s: not a valid shape", str);
		return -1;
	}
	return 0;
}

int cmd_help(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: help";
	Context *ctx = static_cast<Context *>(data);

	if (objc != 1)
		goto fail;
	log_info("available commands:");
	log_info("help: print this");
	log_info("ls [mat|obj|sha]: list materials, objects, and shapes");
	log_info("give <object> [amount]: add objects to inventory");
	log_info("take <object> [amount]: remove objects to inventory");
	log_info("q: query block under the cursor");
	log_info("a: set first corner of selection");
	log_info("b: set second corner of selection");
	log_info("box <block>: fill a box with the given block");
	log_info("hbox <block>: fill a hollow box with the given block");
	log_info("walls <block>: fill the walls of a box with the given block");
	log_info("relit: recalculate lighting for all loaded chunks");
	return TCL_OK;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_ls(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: ls [mat|obj|sha]";
	Context *ctx = static_cast<Context *>(data);
	char *str;

	if (objc < 2)
		goto fail;
	str = Tcl_GetString(objv[1]);
	if (strncmp(str, "mat", 3) == 0) {
		log_info("all materials:");
		for (int i = 0; i < MAT_COUNT; ++i)
			if (mat_names[i])
			log_info("%s", mat_names[i]);
	} else if (strncmp(str, "obj", 3) == 0) {
		log_info("all objects:");
		for (int i = 0; i < OBJ_COUNT; ++i)
			if (obj_names[i])
			log_info("%s", obj_names[i]);
	} else if (strncmp(str, "sha", 3) == 0) {
		log_info("all shapes:");
		for (int i = 0; i < SHAPE_COUNT; ++i)
			if (shape_names[i])
			log_info("%s", shape_names[i]);
	} else {
		goto fail;
	}
	return TCL_OK;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_give(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: give <material>[_<object>] [<amount>]";
	Context *ctx = static_cast<Context *>(data);
	int mat, obj, num = 1;

	if (objc < 2 || objc > 3)
		goto fail;
	if (parse_item(Tcl_GetString(objv[1]), &mat, &obj) != 0)
		goto fail;
	if (objc == 3) {
		if (Tcl_GetIntFromObj(interp, objv[2], &num) != TCL_OK)
			goto fail;
	}
	inventory_add(ctx->player->get_items(), Item(obj, mat, num));
	log_info("given %s_%s %d", mat_names[mat], obj_names[obj], num);
	return 0;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_take(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: take <material>[_<object>] [<amount>]";
	Context *ctx = static_cast<Context *>(data);
	int mat, obj, num = 1;

	if (objc < 2 || objc > 3)
		goto fail;
	if (parse_item(Tcl_GetString(objv[1]), &mat, &obj) != 0)
		goto fail;
	if (objc == 3) {
		if (Tcl_GetIntFromObj(interp, objv[2], &num) != TCL_OK)
			goto fail;
	}
	inventory_remove(ctx->player->get_items(), Item(obj, mat, num));
	log_info("taken %s_%s %d", mat_names[mat], obj_names[obj], num);
	return 0;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_query(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: query";
	Context *ctx = static_cast<Context *>(data);
	const Query &cur = ctx->player->get_cur();

	if (objc != 1)
		goto fail;
	if (cur.face == -1) {
		log_info("no block selected");
		return TCL_OK;
	}
	log_info("block: %lld,%lld,%lld", cur.p.x, cur.p.y, cur.p.z);
	log_info("offset: %.02g,%.02g,%.02g", cur.q.x, cur.q.y, cur.q.z);
	log_info("face: %s", face_names[cur.face]);
	log_info("type: %s.%s",
			mat_names[ctx->world->get_mat(cur.p)],
			shape_names[ctx->world->get_shape(cur.p)]);
	log_info("light level: %d", ctx->world->get_light(cur.p));
/*
	struct array *inv = world_get_data(ctx->w, cur.p);
	if (inv != NULL) {
		log_info("inventory:");
		struct item s;
		int i = 0;
		array_foreach(s, inv) {
			if (s.num > 0)
				log_info("%d: %s_%s %d", i, mat_names[s.mat],
						obj_names[s.obj], s.num);
			++i;
		}
	}
*/
	return TCL_OK;

fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_seta(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: a";
	Context *ctx = static_cast<Context *>(data);
	const Query &cur = ctx->player->get_cur();

	if (objc != 1)
		goto fail;
	if (cur.face == -1) {
		log_info("no block selected");
		return TCL_OK;
	}
	sel_bb.x0 = cur.p.x;
	sel_bb.y0 = cur.p.y;
	sel_bb.z0 = cur.p.z;
	log_info("a set to %lld,%lld,%lld", sel_bb.x0, sel_bb.y0, sel_bb.z0);
	return TCL_OK;

fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_setb(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: b";
	Context *ctx = static_cast<Context *>(data);
	const Query &cur = ctx->player->get_cur();

	if (objc != 1)
		goto fail;
	if (cur.face == -1) {
		log_info("no block selected");
		return TCL_OK;
	}
	sel_bb.x1 = cur.p.x;
	sel_bb.y1 = cur.p.y;
	sel_bb.z1 = cur.p.z;
	log_info("b set to %lld,%lld,%lld", sel_bb.x1, sel_bb.y1, sel_bb.z1);
	return TCL_OK;

fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_box(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: box <material>[_<shape>]";
	Context *ctx = static_cast<Context *>(data);
	int mat, shape;
	box3ll bb;

	if (objc != 2)
		goto fail;
	if (parse_block(Tcl_GetString(objv[1]), &mat, &shape) != 0)
		goto fail;
	bb = fix(sel_bb);
	++bb.x1;
	++bb.y1;
	++bb.z1;
	for (auto &p : bb)
		ctx->world->set_block(p, shape, mat);
	return 0;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_hbox(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: hbox <material>[_<shape>]";
	Context *ctx = static_cast<Context *>(data);
	int mat, shape;
	box3ll bb;
	v3ll p;

	if (objc != 2)
		goto fail;
	if (parse_block(Tcl_GetString(objv[1]), &mat, &shape) != 0)
		goto fail;
	bb = fix(sel_bb);
	for (p.x = bb.x0; p.x <= bb.x1; ++p.x)
		for (p.z = bb.z0; p.z <= bb.z1; ++p.z) {
			p.y = bb.y0;
			ctx->world->set_block(p, shape, mat);
			p.y = bb.y1;
			ctx->world->set_block(p, shape, mat);
		}
	for (p.x = bb.x0; p.x <= bb.x1; ++p.x)
		for (p.y = bb.y0; p.y <= bb.y1; ++p.y) {
			p.z = bb.z0;
			ctx->world->set_block(p, shape, mat);
			p.z = bb.z1;
			ctx->world->set_block(p, shape, mat);
		}
	for (p.y = bb.y0; p.y <= bb.y1; ++p.y)
		for (p.z = bb.z0; p.z <= bb.z1; ++p.z) {
			p.x = bb.x0;
			ctx->world->set_block(p, shape, mat);
			p.x = bb.x1;
			ctx->world->set_block(p, shape, mat);
		}
	return 0;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_walls(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: walls <material>[_<shape>]";
	Context *ctx = static_cast<Context *>(data);
	int mat, shape;
	box3ll bb;
	v3ll p;

	if (objc != 2)
		goto fail;
	if (parse_block(Tcl_GetString(objv[1]), &mat, &shape) != 0)
		goto fail;
	bb = fix(sel_bb);
	for (p.x = bb.x0; p.x <= bb.x1; ++p.x)
		for (p.y = bb.y0; p.y <= bb.y1; ++p.y) {
			p.z = bb.z0;
			ctx->world->set_block(p, shape, mat);
			p.z = bb.z1;
			ctx->world->set_block(p, shape, mat);
		}
	for (p.y = bb.y0; p.y <= bb.y1; ++p.y)
		for (p.z = bb.z0; p.z <= bb.z1; ++p.z) {
			p.x = bb.x0;
			ctx->world->set_block(p, shape, mat);
			p.x = bb.x1;
			ctx->world->set_block(p, shape, mat);
		}
	return 0;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_relit(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: relit";
	Context *ctx = static_cast<Context *>(data);
	box3ll bb, bb2;

	if (objc != 1)
		goto fail;
	bb.x0 = ctx->world->get_p().x;
	bb.y0 = WORLD_H - 1;
	bb.z0 = ctx->world->get_p().y;
	bb.x1 = bb.x0 + WORLD_W;
	bb.y1 = bb.y0 + 1;
	bb.z1 = bb.z0 + WORLD_W;
	update_lighting(ctx->world, bb, &bb2);
	ctx->world->set_flags(bb2, CHUNK_UNRENDERED);
	return TCL_OK;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}
