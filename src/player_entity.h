/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_PLAYER_ENTITY_H_
#define SRC_PLAYER_ENTITY_H_

#include "roaming_entity.h"
#include "main_loop.h"
#include "recipes.h"

class PlayerEntity : public RoamingEntity {
 public:
	explicit PlayerEntity(Context *ctx);
	~PlayerEntity();

	void update();
	void render();
	void load(serializer::Tag *root);
	serializer::Tag *save();

	const char *get_name() { return "player"; }

	inline void set_act(int i) { act = i; }
	inline void set_use(int i) { use = i; }
	inline void set_pick(int i) { pick = i; }
	inline void set_run(int i) { run = i; }
	inline void set_tool(int i) { tool = i; }

	inline void set_move_x0(int i) { move.x0 = i; }
	inline void set_move_y0(int i) { move.y0 = i; }
	inline void set_move_z0(int i) { move.z0 = i; }
	inline void set_move_x1(int i) { move.x1 = i; }
	inline void set_move_y1(int i) { move.y1 = i; }
	inline void set_move_z1(int i) { move.z1 = i; }

	inline int get_act() { return act; }
	inline int get_use() { return use; }
	inline int get_pick() { return pick; }
	inline int get_run() { return run; }
	inline int get_tool() { return tool; }

	inline int get_move_x0() { return move.x0; }
	inline int get_move_y0() { return move.y0; }
	inline int get_move_z0() { return move.z0; }
	inline int get_move_x1() { return move.x1; }
	inline int get_move_y1() { return move.y1; }
	inline int get_move_z1() { return move.z1; }

	inline const Query &get_cur() { return cur; }

	void use_inventory(std::vector<Item> *inv);
	void use_board(std::vector<Item> *inv);
	void use_workbench(std::vector<Item> *inv);

	void handle_event(const SDL_Event &e);

 private:
	box3c move;
	char act;
	char use;
	char pick;
	char run;
	char tool;
	v2c rot;
	Query cur;
	std::vector<RecipeMatch> recipe_matches;
	int selected_recipe;

	void render_cursor();
	void render_held_item();
	void render_hotbar();
	void render_recipe_matches();

	void use_tool();
};

#endif  // SRC_PLAYER_ENTITY_H_
