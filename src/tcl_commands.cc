/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "tcl_commands.h"

#include "context.h"
#include "lighting.h"

static box3ll sel_bb = { 0, 0, 0, 0, 0, 0 };

int parse_item(const char *str, int *mat, int *obj)
{
	for (*mat = MAT_COUNT - 1; *mat >= 0; --*mat) {
		if (mat_names[*mat] && strncmp(str, mat_names[*mat],
					strlen(mat_names[*mat])) == 0)
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
		if (obj_names[*obj] && strncmp(str, obj_names[*obj],
					strlen(obj_names[*obj])) == 0)
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
		*mat = 0;
		*shape = SHAPE_NONE;
		return 0;
	}
	for (*mat = MAT_COUNT - 1; *mat >= 0; --*mat) {
		if (mat_names[*mat] && strncmp(str, mat_names[*mat],
					strlen(mat_names[*mat])) == 0)
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
		if (shape_names[*shape] && strncmp(str, shape_names[*shape],
					strlen(shape_names[*shape])) == 0)
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
	log_info("tp <x> <y> <z>: teleport to coordinates");
	log_info("give <object> [amount]: add objects to inventory");
	log_info("take <object> [amount]: remove objects to inventory");
	log_info("q: query block under the cursor");
	log_info("a: set first corner of selection");
	log_info("b: set second corner of selection");
	log_info("box <block>: fill a box with the given block");
	log_info("hbox <block>: fill a hollow box with the given block");
	log_info("walls <block>: fill the walls of a box with the given block");
	log_info("relit: recalculate lighting for current selection");
	log_info("replace <from-block> <to-block>: replace blocks in region");
	return TCL_OK;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_ls(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: ls [item|block]";
	Context *ctx = static_cast<Context *>(data);
	char *str;

	if (objc < 2)
		goto fail;
	str = Tcl_GetString(objv[1]);
	if (strcmp(str, "item") == 0) {
		log_info("all items:");
		for (int i = 0; i < MAT_COUNT; ++i) {
			if (mat_names[i]) {
				for (int j = 0; j < OBJ_COUNT; ++j) {
					if (obj_names[j]) {
						log_info("%s_%s", mat_names[i],
								obj_names[j]);
					}
				}
			}
		}
	} else if (strcmp(str, "block") == 0) {
		log_info("all blocks:");
		for (int i = 0; i < MAT_COUNT; ++i) {
			if (mat_names[i]) {
				for (int j = 0; j < SHAPE_COUNT; ++j) {
					if (shape_names[j]) {
						log_info("%s_%s", mat_names[i],
								shape_names[j]);
					}
				}
			}
		}
	} else {
		goto fail;
	}
	return TCL_OK;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_tp(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: tp <x> <y> <z>";
	Context *ctx = static_cast<Context *>(data);
	int x, y, z;

	if (objc != 4)
		goto fail;
	if (Tcl_GetIntFromObj(interp, objv[1], &x) != TCL_OK)
		goto fail;
	if (Tcl_GetIntFromObj(interp, objv[2], &y) != TCL_OK)
		goto fail;
	if (Tcl_GetIntFromObj(interp, objv[3], &z) != TCL_OK)
		goto fail;
	ctx->player->get_body()->set_p(v3f(x, y, z));
	return TCL_OK;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_give(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: give <item> [<amount>]";
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
	num = inventory_add(ctx->player->get_items(), Item(obj, mat, num));
	Tcl_SetObjResult(interp, Tcl_NewIntObj(num));
	return 0;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_take(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: take <item> [<amount>]";
	Context *ctx = static_cast<Context *>(data);
	int mat, obj, num = 1;

	if (objc < 2 || objc > 3)
		goto fail;
	if (strcmp(Tcl_GetString(objv[1]), "all") == 0) {
		for (auto &i : *ctx->player->get_items())
			i.num = 0;
		return TCL_OK;
	}
	if (parse_item(Tcl_GetString(objv[1]), &mat, &obj) != 0)
		goto fail;
	if (objc == 3) {
		if (Tcl_GetIntFromObj(interp, objv[2], &num) != TCL_OK)
			goto fail;
	}
	num = inventory_remove(ctx->player->get_items(), Item(obj, mat, num));
	Tcl_SetObjResult(interp, Tcl_NewIntObj(num));
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
	char buf[256];
	int len = 0;

	if (objc != 1)
		goto fail;
	if (cur.face == -1) {
		log_info("no block selected");
		return TCL_OK;
	}
	len += snprintf(buf + len, sizeof(buf) + len,
			"block: %lld,%lld,%lld\n", cur.p.x, cur.p.y, cur.p.z);
	len += snprintf(buf + len, sizeof(buf) + len,
			"offset: %.02g,%.02g,%.02g\n", cur.q.x, cur.q.y, cur.q.z);
	len += snprintf(buf + len, sizeof(buf) + len,
			"face: %s\n", face_names[cur.face]);
	len += snprintf(buf + len, sizeof(buf) + len,
			"type: %s.%s\n",
			mat_names[ctx->world->get_mat(cur.p)],
			shape_names[ctx->world->get_shape(cur.p)]);
	len += snprintf(buf + len, sizeof(buf) + len,
			"light level: %d\n", ctx->world->get_light(cur.p));
	Tcl_SetObjResult(interp, Tcl_NewStringObj(buf, len));
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
	char buf[64];
	snprintf(buf, sizeof(buf), "%lld %lld %lld", sel_bb.x0, sel_bb.y0, sel_bb.z0);
	Tcl_SetObjResult(interp, Tcl_NewStringObj(buf, strlen(buf)));
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
	char buf[64];
	snprintf(buf, sizeof(buf), "%lld %lld %lld", sel_bb.x1, sel_bb.y1, sel_bb.z1);
	Tcl_SetObjResult(interp, Tcl_NewStringObj(buf, strlen(buf)));
	return TCL_OK;

fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_box(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: box <block>";
	Context *ctx = static_cast<Context *>(data);
	int mat, shape, ret = 0;
	box3ll bb;

	if (objc != 2)
		goto fail;
	if (parse_block(Tcl_GetString(objv[1]), &mat, &shape) != 0)
		goto fail;
	bb = fix(sel_bb);
	for (auto &p : bb) {
		ctx->world->set_block(p, shape, mat);
		++ret;
	}
	Tcl_SetObjResult(interp, Tcl_NewIntObj(ret));
	return 0;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_hbox(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: hbox <block>";
	Context *ctx = static_cast<Context *>(data);
	int mat, shape, ret = 0;
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
			ret += 2;
		}
	for (p.x = bb.x0; p.x <= bb.x1; ++p.x)
		for (p.y = bb.y0; p.y <= bb.y1; ++p.y) {
			p.z = bb.z0;
			ctx->world->set_block(p, shape, mat);
			p.z = bb.z1;
			ctx->world->set_block(p, shape, mat);
			ret += 2;
		}
	for (p.y = bb.y0; p.y <= bb.y1; ++p.y)
		for (p.z = bb.z0; p.z <= bb.z1; ++p.z) {
			p.x = bb.x0;
			ctx->world->set_block(p, shape, mat);
			p.x = bb.x1;
			ctx->world->set_block(p, shape, mat);
			ret += 2;
		}
	Tcl_SetObjResult(interp, Tcl_NewIntObj(ret));
	return 0;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_walls(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: walls <block>";
	Context *ctx = static_cast<Context *>(data);
	int mat, shape, ret = 0;
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
			ret += 2;
		}
	for (p.y = bb.y0; p.y <= bb.y1; ++p.y)
		for (p.z = bb.z0; p.z <= bb.z1; ++p.z) {
			p.x = bb.x0;
			ctx->world->set_block(p, shape, mat);
			p.x = bb.x1;
			ctx->world->set_block(p, shape, mat);
			ret += 2;
		}
	Tcl_SetObjResult(interp, Tcl_NewIntObj(ret));
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
	bb = fix(sel_bb);
	ctx->light->update(bb, &bb2);
	ctx->world->set_flags(bb2, Chunk::UNRENDERED);
	return TCL_OK;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_replace(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: replace <from-block> <to-block>";
	Context *ctx = static_cast<Context *>(data);
	int m1, s1, m2, s2;
	box3ll bb;

	if (objc != 3)
		goto fail;
	if (parse_block(Tcl_GetString(objv[1]), &m1, &s1) != 0)
		goto fail;
	if (parse_block(Tcl_GetString(objv[2]), &m2, &s2) != 0)
		goto fail;
	bb = fix(sel_bb);
	for (auto &p : bb) {
		if (ctx->world->get_mat(p) == m1
				&& ctx->world->get_shape(p) == s1)
			ctx->world->set_block(p, s2, m2);
	}
	return 0;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

