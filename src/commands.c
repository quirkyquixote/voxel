
#include "commands.h"

#include "context.h"

int cmd_give(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: give <material> <object> <amount>";
	struct context *ctx = data;
	int mat, obj, num;

	if (objc != 4)
		goto fail;
	mat = mat_from_name(Tcl_GetString(objv[1]));
	if (mat < 0)
		goto fail;
	obj = obj_from_name(Tcl_GetString(objv[2]));
	if (obj < 0)
		goto fail;
	if (Tcl_GetIntFromObj(interp, objv[3], &num) != 0)
		goto fail;
	inventory_add(ctx->inv, slot(obj, mat, num));
	return 0;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

int cmd_take(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: take <material> <object> <amount>";
	struct context *ctx = data;
	int mat, obj, num;

	if (objc != 4)
		goto fail;
	mat = mat_from_name(Tcl_GetString(objv[1]));
	if (mat < 0)
		goto fail;
	obj = obj_from_name(Tcl_GetString(objv[2]));
	if (obj < 0)
		goto fail;
	if (Tcl_GetIntFromObj(interp, objv[3], &num) != 0)
		goto fail;
	inventory_remove(ctx->inv, slot(obj, mat, num));
	return 0;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

