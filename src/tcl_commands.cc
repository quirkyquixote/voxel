/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "tcl_commands.h"

#include "context.h"
#include "lighting.h"

static box3ll sel_bb = { 0, 0, 0, 0, 0, 0 };

inline void parse_argument(Tcl_Interp *interp, Tcl_Obj *obj, std::string *ret)
{
	*ret = Tcl_GetString(obj);
}

inline void parse_argument(Tcl_Interp *interp, Tcl_Obj *obj, int *rval)
{
	if (Tcl_GetIntFromObj(interp, obj, rval) != TCL_OK) {
		Tcl_ResetResult(interp);
		throw Exception("Expected integer at \"%s\"", Tcl_GetString(obj));
	}
}

inline void parse_argument(Tcl_Interp *interp, Tcl_Obj *obj, double *rval)
{
	if (Tcl_GetDoubleFromObj(interp, obj, rval) != TCL_OK) {
		Tcl_ResetResult(interp);
		throw Exception("Expected double at \"%s\"", Tcl_GetString(obj));
	}
}

template<typename T>
int parse_arguments(Tcl_Interp *interp, int objc, Tcl_Obj *const objv[], T *t)
{
	if (objc < 1)
		return 0;
	if (objc > 1)
		throw Exception("Too many arguments");
	parse_argument(interp, *objv, t);
	return 1;
}

template<typename T, typename...A>
int parse_arguments(Tcl_Interp *interp, int objc, Tcl_Obj *const objv[], T *t, A...args)
{
	if (objc < 1)
		return 0;
	parse_argument(interp, *objv, t);
	return parse_arguments(interp, --objc, ++objv, args...) + 1;
}

void parse_item(const char *str, int *mat, int *obj)
{
	for (*mat = MAT_COUNT - 1; *mat >= 0; --*mat) {
		if (mat_names[*mat] && strncmp(str, mat_names[*mat],
					strlen(mat_names[*mat])) == 0)
			break;
	}
	if (*mat < 0)
		throw Exception("Expected item at \"%s\"", str);
	const char *ptr = str + strlen(mat_names[*mat]);
	if (*ptr == 0) {
		*obj = 0;
		return;
	}
	if (*ptr != '_')
		throw Exception("Expected item at \"%s\"", str);
	++ptr;
	for (*obj = OBJ_COUNT - 1; *obj >= 0; --*obj) {
		if (obj_names[*obj] && strncmp(ptr, obj_names[*obj],
					strlen(obj_names[*obj])) == 0)
			break;
	}
	if (*obj < 0)
		throw Exception("Expected item at \"%s\"", str);
}

void parse_block(const char *str, int *mat, int *shape)
{
	if (strcmp(str, "air") == 0) {
		*mat = 0;
		*shape = SHAPE_NONE;
		return;
	}
	for (*mat = MAT_COUNT - 1; *mat >= 0; --*mat) {
		if (mat_names[*mat] && strncmp(str, mat_names[*mat],
					strlen(mat_names[*mat])) == 0)
			break;
	}
	if (*mat < 0)
		throw Exception("Expected block at \"%s\"", str);
	const char *ptr = str + strlen(mat_names[*mat]);
	if (*ptr == 0) {
		*shape = SHAPE_BLOCK_DN;
		return;
	}
	if (*ptr != '_')
		throw Exception("Expected block at \"%s\"", str);
	++ptr;
	for (*shape = SHAPE_COUNT - 1; *shape >= 0; --*shape) {
		if (shape_names[*shape] && strncmp(ptr, shape_names[*shape],
					strlen(shape_names[*shape])) == 0)
			break;
	}
	if (*shape < 0)
		throw Exception("Expected block at \"%s\"", str);
}

int cmd_tp(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	Context *ctx = static_cast<Context *>(data);
	int x, y, z;

	try {
		if (parse_arguments(interp, objc - 1, objv + 1, &x, &y, &z) < 3)
			throw Exception("Missing arguments");
		ctx->player->get_body()->set_p(v3f(x, y, z));
		return TCL_OK;
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, "tp <x> <y> <z>: ", ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_give(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	Context *ctx = static_cast<Context *>(data);
	std::string item;
	int mat, obj, num = 1;

	try {
		if (parse_arguments(interp, objc - 1, objv + 1, &item, &num) < 1)
			throw Exception("Missing arguments");
		parse_item(item.c_str(), &mat, &obj);
		num = inventory_add(ctx->player->get_items(), Item(obj, mat, num));
		Tcl_SetObjResult(interp, Tcl_NewIntObj(num));
		return TCL_OK;
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, "give <item> [<amount>]: ", ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_take(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	Context *ctx = static_cast<Context *>(data);
	std::string item;
	int mat, obj, num = 1;

	try {
		if (parse_arguments(interp, objc - 1, objv + 1, &item, &num) < 1)
			throw Exception("Missing arguments");
		if (item == "all") {
			for (auto &i : *ctx->player->get_items())
				i.num = 0;
			return TCL_OK;
		}
		parse_item(item.c_str(), &mat, &obj);
		num = inventory_remove(ctx->player->get_items(), Item(obj, mat, num));
		Tcl_SetObjResult(interp, Tcl_NewIntObj(num));
		return 0;
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, "take <item> [<amount>]: ", ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_query(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	Context *ctx = static_cast<Context *>(data);
	const Query &cur = ctx->player->get_cur();
	char buf[256];
	int len = 0;

	try {
		if (objc != 1)
			throw Exception("Too many arguments");
		if (cur.face == -1)
			throw Exception("No block selected");
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
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, "query: ", ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_seta(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	Context *ctx = static_cast<Context *>(data);
	const Query &cur = ctx->player->get_cur();

	try {
		if (objc != 1)
			throw Exception("Too many arguments");
		if (cur.face == -1)
			throw Exception("No block selected");
		sel_bb.x0 = cur.p.x;
		sel_bb.y0 = cur.p.y;
		sel_bb.z0 = cur.p.z;
		char buf[64];
		snprintf(buf, sizeof(buf), "%lld %lld %lld", sel_bb.x0, sel_bb.y0, sel_bb.z0);
		Tcl_SetObjResult(interp, Tcl_NewStringObj(buf, strlen(buf)));
		return TCL_OK;
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, "a: ", ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_setb(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	Context *ctx = static_cast<Context *>(data);
	const Query &cur = ctx->player->get_cur();

	try {
		if (objc != 1)
			throw Exception("Too many arguments");
		if (cur.face == -1)
			throw Exception("No block selected");
		sel_bb.x1 = cur.p.x;
		sel_bb.y1 = cur.p.y;
		sel_bb.z1 = cur.p.z;
		char buf[64];
		snprintf(buf, sizeof(buf), "%lld %lld %lld", sel_bb.x1, sel_bb.y1, sel_bb.z1);
		Tcl_SetObjResult(interp, Tcl_NewStringObj(buf, strlen(buf)));
		return TCL_OK;
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, "b: ", ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_box(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	Context *ctx = static_cast<Context *>(data);
	std::string block;
	int mat, shape, ret = 0;
	box3ll bb;

	try {
		if (parse_arguments(interp, objc - 1, objv + 1, &block) < 1)
			throw Exception("Missing arguments");
		parse_block(block.c_str(), &mat, &shape);
		bb = fix(sel_bb);
		for (auto &p : bb) {
			ctx->world->set_block(p, shape, mat);
			++ret;
		}
		Tcl_SetObjResult(interp, Tcl_NewIntObj(ret));
		return 0;
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, "box <block>: ", ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_hbox(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	Context *ctx = static_cast<Context *>(data);
	std::string block;
	int mat, shape, ret = 0;
	box3ll bb;
	v3ll p;

	try {
		if (parse_arguments(interp, objc - 1, objv + 1, &block) < 1)
			throw Exception("Missing arguments");
		parse_block(block.c_str(), &mat, &shape);
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
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, "hbox <block>: ", ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_walls(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	Context *ctx = static_cast<Context *>(data);
	std::string block;
	int mat, shape, ret = 0;
	box3ll bb;
	v3ll p;

	try {
		if (parse_arguments(interp, objc - 1, objv + 1, &block) < 1)
			throw Exception("Missing arguments");
		parse_block(block.c_str(), &mat, &shape);
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
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, "walls <block>: ", ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_relit(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	Context *ctx = static_cast<Context *>(data);
	box3ll bb, bb2;

	try {
		if (objc != 1)
			throw Exception("Too many arguments");
		bb = fix(sel_bb);
		ctx->light->update(bb, &bb2);
		ctx->world->set_flags(bb2, Chunk::UNRENDERED);
		return TCL_OK;
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, "relit: ", ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_replace(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	Context *ctx = static_cast<Context *>(data);
	std::string block1, block2;
	int m1, s1, m2, s2;
	box3ll bb;

	try {
		if (parse_arguments(interp, objc - 1, objv + 1, &block1, &block2) < 3)
			throw Exception("Missing arguments");
		parse_block(block1.c_str(), &m1, &s1);
		parse_block(block2.c_str(), &m2, &s2);
		bb = fix(sel_bb);
		for (auto &p : bb) {
			if (ctx->world->get_mat(p) == m1
					&& ctx->world->get_shape(p) == s1)
				ctx->world->set_block(p, s2, m2);
		}
		return TCL_OK;
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, "replace <from-block> <to-block>: ", ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_regen(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	Context *ctx = static_cast<Context *>(data);
	v3f p;
	v2ll q;
	Chunk *c;

	try {
		if (objc != 1)
			throw Exception("Too many arguments");
		p = ctx->player->get_body()->get_p();
		q = (v2ll(p.x, p.z) >> 4LL) & 0xfLL;
		c = ctx->world->get_chunk(q);
		terraform(0, c);
		c->set_flags(Chunk::UNLIT);
		return TCL_OK;
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, "terraform: ", ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

