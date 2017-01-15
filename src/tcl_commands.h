
#ifndef VOXEL_COMMANDS_H_
#define VOXEL_COMMANDS_H_

#include <tcl.h>

int cmd_help(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_ls(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_give(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_take(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_seta(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_setb(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_box(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_hbox(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_walls(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_query(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_relit(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);

#endif
