
#include "commands.h"

#include "context.h"

int parse_slot(const char *str, int *mat, int *obj)
{
	char *tmp;
	char *ptr;
	tmp = strdup(str);
	ptr = strchr(tmp, '.');
	if (ptr == NULL) {
		*mat = mat_from_name(tmp);
		*obj = 0;
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
	log_info("given %s %s %d", mat_names[mat], obj_names[obj], num);
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
	log_info("taken %s %s %d", mat_names[mat], obj_names[obj], num);
	return 0;
fail:
	Tcl_SetObjResult(interp, Tcl_NewStringObj(usage, strlen(usage)));
	return TCL_ERROR;
}

