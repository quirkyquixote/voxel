
#ifndef VOXEL_COMMANDS_H_
#define VOXEL_COMMANDS_H_

#include <tcl.h>

int cmd_give(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_take(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_fill(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_query(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_relit(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);

#endif
