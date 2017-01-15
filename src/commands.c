
#include "commands.h"

#include "context.h"
#include "lighting.h"

int parse_slot(const char *str, int *mat, int *obj)
{
	char *tmp;
	char *ptr;
	tmp = strdup(str);
	ptr = strchr(tmp, '.');
	if (ptr == NULL) {
		*mat = mat_from_name(tmp);
		*obj = OBJ_BLOCK;
	} else {
		*ptr = 0;
		++ptr;
		*mat = mat_from_name(tmp);
		*obj = obj_from_name(ptr);
	}
	free(tmp);
	if (*mat < 0 || *obj < 0)
		return -1;
	return 0;
}

int parse_block(const char *str, int *mat, int *shape)
{
	char *tmp;
	char *ptr;
	tmp = strdup(str);
	ptr = strchr(tmp, '.');
	if (ptr == NULL) {
		*mat = mat_from_name(tmp);
		*shape = SHAPE_BLOCK_DN;
	} else {
		*ptr = 0;
		++ptr;
		*mat = mat_from_name(tmp);
		*shape = shape_from_name(ptr);
	}
	free(tmp);
	if (*mat < 0 || *shape < 0)
		return -1;
	return 0;
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
		if (Tcl_GetIntFromObj(interp, objv[3], &num) != 0)
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
		if (Tcl_GetIntFromObj(interp, objv[3], &num) != 0)
			goto fail;
	}
	inventory_remove(ctx->inv, slot(obj, mat, num));
	log_info("taken %s.%s %d", mat_names[mat], obj_names[obj], num);
	return 0;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_fill(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: fill <material>[.<shape>]";
	struct context *ctx = data;
	int mat, shape;

	if (objc != 2)
		goto fail;
	if (parse_block(Tcl_GetString(objv[1]), &mat, &shape) != 0)
		goto fail;
	log_info("fill %s.%s", mat_names[mat], shape_names[shape]);
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
