/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "tcl_commands.h"

#include "context.h"
#include "lighting.h"

static box3ll sel_bb = { 0, 0, 0, 0, 0, 0 };

void parse_item(const char *str, int *mat, int *obj)
{
	for (*mat = MAT_COUNT - 1; *mat >= 0; --*mat) {
		if (mat_names[*mat] && strncmp(str, mat_names[*mat],
					strlen(mat_names[*mat])) == 0)
			break;
	}
	if (*mat < 0)
		throw Exception("%s: not a valid material", str);
	str += strlen(mat_names[*mat]);
	if (*str == 0) {
		*obj = 0;
		return;
	}
	if (*str != '_')
		throw Exception("expected '_' after %s", mat_names[*mat]);
	++str;
	for (*obj = OBJ_COUNT - 1; *obj >= 0; --*obj) {
		if (obj_names[*obj] && strncmp(str, obj_names[*obj],
					strlen(obj_names[*obj])) == 0)
			break;
	}
	if (*obj < 0)
		throw Exception("%s: not a valid object", str);
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
		throw Exception("%s: not a valid material", str);
	str += strlen(mat_names[*mat]);
	if (*str == 0) {
		*shape = SHAPE_BLOCK_DN;
		return;
	}
	if (*str != '_')
		throw Exception("expected '_' after %s", mat_names[*mat]);
	++str;
	for (*shape = SHAPE_COUNT - 1; *shape >= 0; --*shape) {
		if (shape_names[*shape] && strncmp(str, shape_names[*shape],
					strlen(shape_names[*shape])) == 0)
			break;
	}
	if (*shape < 0)
		throw Exception("%s: not a valid shape", str);
}

int cmd_tp(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: tp <x> <y> <z>";
	Context *ctx = static_cast<Context *>(data);
	int x, y, z;

	try {
		if (objc != 4)
			throw Exception(usage);
		if (Tcl_GetIntFromObj(interp, objv[1], &x) != TCL_OK)
			throw Exception();
		if (Tcl_GetIntFromObj(interp, objv[2], &y) != TCL_OK)
			throw Exception();
		if (Tcl_GetIntFromObj(interp, objv[3], &z) != TCL_OK)
			throw Exception();
		ctx->player->get_body()->set_p(v3f(x, y, z));
		return TCL_OK;
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_give(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: give <item> [<amount>]";
	Context *ctx = static_cast<Context *>(data);
	int mat, obj, num = 1;

	try {
		if (objc < 2 || objc > 3)
			throw Exception(usage);
		parse_item(Tcl_GetString(objv[1]), &mat, &obj);
		if (objc == 3) {
			if (Tcl_GetIntFromObj(interp, objv[2], &num) != TCL_OK)
				throw Exception();
		}
		num = inventory_add(ctx->player->get_items(), Item(obj, mat, num));
		Tcl_SetObjResult(interp, Tcl_NewIntObj(num));
		return TCL_OK;
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_take(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: take <item> [<amount>]";
	Context *ctx = static_cast<Context *>(data);
	int mat, obj, num = 1;

	try {
		if (objc < 2 || objc > 3)
			throw Exception(usage);
		if (strcmp(Tcl_GetString(objv[1]), "all") == 0) {
			for (auto &i : *ctx->player->get_items())
				i.num = 0;
			return TCL_OK;
		}
		parse_item(Tcl_GetString(objv[1]), &mat, &obj);
		if (objc == 3) {
			if (Tcl_GetIntFromObj(interp, objv[2], &num) != TCL_OK)
				throw Exception();
		}
		num = inventory_remove(ctx->player->get_items(), Item(obj, mat, num));
		Tcl_SetObjResult(interp, Tcl_NewIntObj(num));
		return 0;
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_query(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: query";
	Context *ctx = static_cast<Context *>(data);
	const Query &cur = ctx->player->get_cur();
	char buf[256];
	int len = 0;

	try {
		if (objc != 1)
			throw Exception(usage);
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
		Tcl_AppendResult(interp, ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_seta(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: a";
	Context *ctx = static_cast<Context *>(data);
	const Query &cur = ctx->player->get_cur();

	try {
		if (objc != 1)
			throw Exception(usage);
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
		Tcl_AppendResult(interp, ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_setb(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: b";
	Context *ctx = static_cast<Context *>(data);
	const Query &cur = ctx->player->get_cur();

	try {
		if (objc != 1)
			throw Exception(usage);
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
		Tcl_AppendResult(interp, ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_box(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: box <block>";
	Context *ctx = static_cast<Context *>(data);
	int mat, shape, ret = 0;
	box3ll bb;

	try {
		if (objc != 2)
			throw Exception(usage);
		parse_block(Tcl_GetString(objv[1]), &mat, &shape);
		bb = fix(sel_bb);
		for (auto &p : bb) {
			ctx->world->set_block(p, shape, mat);
			++ret;
		}
		Tcl_SetObjResult(interp, Tcl_NewIntObj(ret));
		return 0;
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_hbox(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: hbox <block>";
	Context *ctx = static_cast<Context *>(data);
	int mat, shape, ret = 0;
	box3ll bb;
	v3ll p;

	try {
		if (objc != 2)
			throw Exception(usage);
		parse_block(Tcl_GetString(objv[1]), &mat, &shape);
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
		Tcl_AppendResult(interp, ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_walls(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: walls <block>";
	Context *ctx = static_cast<Context *>(data);
	int mat, shape, ret = 0;
	box3ll bb;
	v3ll p;

	try {
		if (objc != 2)
			throw Exception(usage);
		parse_block(Tcl_GetString(objv[1]), &mat, &shape);
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
		Tcl_AppendResult(interp, ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_relit(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: relit";
	Context *ctx = static_cast<Context *>(data);
	box3ll bb, bb2;

	try {
		if (objc != 1)
			throw Exception(usage);
		bb = fix(sel_bb);
		ctx->light->update(bb, &bb2);
		ctx->world->set_flags(bb2, Chunk::UNRENDERED);
		return TCL_OK;
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_replace(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: replace <from-block> <to-block>";
	Context *ctx = static_cast<Context *>(data);
	int m1, s1, m2, s2;
	box3ll bb;

	try {
		if (objc != 3)
			throw Exception(usage);
		parse_block(Tcl_GetString(objv[1]), &m1, &s1);
		parse_block(Tcl_GetString(objv[2]), &m2, &s2);
		bb = fix(sel_bb);
		for (auto &p : bb) {
			if (ctx->world->get_mat(p) == m1
					&& ctx->world->get_shape(p) == s1)
				ctx->world->set_block(p, s2, m2);
		}
		return TCL_OK;
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

int cmd_regen(void *data, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	static const char *usage = "usage: terraform";
	Context *ctx = static_cast<Context *>(data);
	v3f p;
	v2ll q;
	Chunk *c;

	try {
		if (objc != 1)
			throw Exception(usage);
		p = ctx->player->get_body()->get_p();
		q = (v2ll(p.x, p.z) >> 4LL) & 0xfLL;
		c = ctx->world->get_chunk(q);
		terraform(0, c);
		c->set_flags(Chunk::UNLIT);
		return TCL_OK;
	} catch (Exception &ex) {
		Tcl_AppendResult(interp, ex.get_str(), NULL);
		return TCL_ERROR;
	}
}

