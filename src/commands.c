
#include "commands.h"

#include "context.h"
#include "lighting.h"

static struct box3ll sel_bb = { 0, 0, 0, 0, 0, 0 };

int parse_slot(const char *str, int *mat, int *obj)
{
	char *tmp, *ptr;
	int ret;

	tmp = strdup(str);
	ptr = strchr(tmp, '.');
	ret = 0;
	if (ptr == NULL) {
		*mat = mat_from_name(tmp);
		if (*mat < 0) {
			log_warning("%s: not a valid material", tmp);
			ret = -1;
		}
		*obj = OBJ_BLOCK;
	} else {
		*ptr = 0;
		++ptr;
		*mat = mat_from_name(tmp);
		if (*mat < 0) {
			log_warning("%s: not a valid material", tmp);
			ret = -1;
		}
		*obj = obj_from_name(ptr);
		if (*obj < 0) {
			log_warning("%s: not a valid object", ptr);
			ret = -1;
		}
	}
	free(tmp);
	return ret;
}

int parse_block(const char *str, int *mat, int *shape)
{
	char *tmp, *ptr;
	int ret;

	tmp = strdup(str);
	ptr = strchr(tmp, '.');
	ret = 0;
	if (ptr == NULL) {
		*mat = mat_from_name(tmp);
		if (*mat < 0) {
			log_warning("%s: not a valid material", tmp);
			ret = -1;
		}
		*shape = SHAPE_BLOCK_DN;
	} else {
		*ptr = 0;
		++ptr;
		*mat = mat_from_name(tmp);
		if (*mat < 0) {
			log_warning("%s: not a valid material", tmp);
			ret = -1;
		}
		*shape = shape_from_name(ptr);
		if (*shape < 0) {
			log_warning("%s: not a valid shape", ptr);
			ret = -1;
		}
	}
	free(tmp);
	return ret;
}

int cmd_ls(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: ls [mat|obj|sha]";
	struct context *ctx = data;
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
	static const char *usage = "usage: give <material>[.<object>] [<amount>]";
	struct context *ctx = data;
	int mat, obj, num = 1;

	if (objc < 2 || objc > 3)
		goto fail;
	if (parse_slot(Tcl_GetString(objv[1]), &mat, &obj) != 0)
		goto fail;
	if (objc == 3) {
		if (Tcl_GetIntFromObj(interp, objv[2], &num) != TCL_OK)
			goto fail;
	}
	inventory_add(ctx->inv, slot(obj, mat, num));
	log_info("given %s.%s %d", mat_names[mat], obj_names[obj], num);
	return 0;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_take(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: take <material>[.<object>] [<amount>]";
	struct context *ctx = data;
	int mat, obj, num = 1;

	if (objc < 2 || objc > 3)
		goto fail;
	if (parse_slot(Tcl_GetString(objv[1]), &mat, &obj) != 0)
		goto fail;
	if (objc == 3) {
		if (Tcl_GetIntFromObj(interp, objv[2], &num) != TCL_OK)
			goto fail;
	}
	inventory_remove(ctx->inv, slot(obj, mat, num));
	log_info("taken %s.%s %d", mat_names[mat], obj_names[obj], num);
	return 0;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_query(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: query";
	struct context *ctx = data;

	if (objc != 1)
		goto fail;
	if (ctx->cur.face == -1) {
		log_info("no block selected");
		return TCL_OK;
	}
	log_info("block: %d,%d,%d", ctx->cur.p.x, ctx->cur.p.y, ctx->cur.p.z);
	log_info("offset: %.02g,%.02g,%.02g", ctx->cur.q.x, ctx->cur.q.y, ctx->cur.q.z);
	log_info("face: %s", face_names[ctx->cur.face]);
	log_info("type: %s.%s",
			mat_names[world_get_mat(ctx->w, ctx->cur.p)],
			shape_names[world_get_shape(ctx->w, ctx->cur.p)]);
	log_info("light level: %d", world_get_light(ctx->w, ctx->cur.p));
	struct array *inv = world_get_data(ctx->w, ctx->cur.p);
	if (inv != NULL) {
		log_info("inventory:");
		struct slot s;
		int i = 0;
		array_foreach(s, inv) {
			if (s.num > 0)
				log_info("%d: %s %s %d", i, mat_names[s.mat],
						obj_names[s.obj], s.num);
			++i;
		}
	}
	return TCL_OK;

fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_seta(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: a";
	struct context *ctx = data;

	if (objc != 1)
		goto fail;
	if (ctx->cur.face == -1) {
		log_info("no block selected");
		return TCL_OK;
	}
	sel_bb.x0 = ctx->cur.p.x;
	sel_bb.y0 = ctx->cur.p.y;
	sel_bb.z0 = ctx->cur.p.z;
	log_info("a set to %lld,%lld,%lld", sel_bb.x0, sel_bb.y0, sel_bb.z0);
	return TCL_OK;

fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_setb(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: b";
	struct context *ctx = data;

	if (objc != 1)
		goto fail;
	if (ctx->cur.face == -1) {
		log_info("no block selected");
		return TCL_OK;
	}
	sel_bb.x1 = ctx->cur.p.x;
	sel_bb.y1 = ctx->cur.p.y;
	sel_bb.z1 = ctx->cur.p.z;
	log_info("b set to %lld,%lld,%lld", sel_bb.x1, sel_bb.y1, sel_bb.z1);
	return TCL_OK;

fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_box(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: box <material>[.<shape>]";
	struct context *ctx = data;
	int mat, shape;
	struct box3ll bb;
	struct v3ll p;

	if (objc != 2)
		goto fail;
	if (parse_block(Tcl_GetString(objv[1]), &mat, &shape) != 0)
		goto fail;
	bb = box3_fix(sel_bb);
	++bb.x1;
	++bb.y1;
	++bb.z1;
	box3_foreach(p, bb)
		world_set(ctx->w, p, shape, mat, NULL);
	return 0;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_hbox(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: hbox <material>[.<shape>]";
	struct context *ctx = data;
	int mat, shape;
	struct box3ll bb;
	struct v3ll p;

	if (objc != 2)
		goto fail;
	if (parse_block(Tcl_GetString(objv[1]), &mat, &shape) != 0)
		goto fail;
	bb = box3_fix(sel_bb);
	for (p.x = bb.x0; p.x <= bb.x1; ++p.x)
		for (p.z = bb.z0; p.z <= bb.z1; ++p.z) {
			p.y = bb.y0;
			world_set(ctx->w, p, shape, mat, NULL);
			p.y = bb.y1;
			world_set(ctx->w, p, shape, mat, NULL);
		}
	for (p.x = bb.x0; p.x <= bb.x1; ++p.x)
		for (p.y = bb.y0; p.y <= bb.y1; ++p.y) {
			p.z = bb.z0;
			world_set(ctx->w, p, shape, mat, NULL);
			p.z = bb.z1;
			world_set(ctx->w, p, shape, mat, NULL);
		}
	for (p.y = bb.y0; p.y <= bb.y1; ++p.y)
		for (p.z = bb.z0; p.z <= bb.z1; ++p.z) {
			p.x = bb.x0;
			world_set(ctx->w, p, shape, mat, NULL);
			p.x = bb.x1;
			world_set(ctx->w, p, shape, mat, NULL);
		}
	return 0;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_relit(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: relit";
	struct context *ctx = data;

	if (objc != 1)
		goto fail;
	struct box3ll bb, bb2;
	bb.x0 = ctx->w->x;
	bb.y0 = WORLD_H - 1;
	bb.z0 = ctx->w->z;
	bb.x1 = bb.x0 + WORLD_W;
	bb.y1 = bb.y0 + 1;
	bb.z1 = bb.z0 + WORLD_W;
	update_lighting(ctx->w, bb, &bb2);
	world_set_flags(ctx->w, bb2, CHUNK_UNRENDERED);
	return TCL_OK;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}
