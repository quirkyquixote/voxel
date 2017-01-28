/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_TCL_COMMANDS_H_
#define SRC_TCL_COMMANDS_H_

#include <tcl.h>

int cmd_tp(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_give(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_take(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_seta(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_setb(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_box(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_hbox(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_walls(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_query(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_relit(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_replace(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int cmd_terraform(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);

#endif  // SRC_TCL_COMMANDS_H_
