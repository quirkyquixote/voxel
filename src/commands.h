
#ifndef VOXEL_COMMANDS_H_
#define VOXEL_COMMANDS_H_

#include <tcl.h>

#include "array.h"
#include "str.h"

struct commandline {
	struct str* buf;
	struct array *history;
	char *visible;
	Tcl_Interp *tcl;
};

struct commandline *commandline(void);

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
