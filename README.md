# Yet Another MineCraft-ish Engine

I've been playing Minecraft on and off for quite a long time, and seen how the
game evolved through time; the problem is, it didn't evolve like I hoped it
did.

I had ideas. I asked myself...

Why aren't faraway chunks rendered in a simplified way so the scenery appears
bigger and mountains aren't lost in the distance?

Why are redstone-based state machines so clunky? simple logic gates are so big
that minimally complex mechanisms can't be implemented in a practical manner.

And could be done if the core was to be implemented from the beginning so
non-cubic blocks were actually supported?

So finally, at the beginning of 2017 I started coding in my spare time. This is
the result.

I'll keep updating the thing and adding stuff as I get the time to do so.

## Requirements

The following libraries are needed to compile this:

- SDL2, including SDL_image, SDL_mixer, and SDL_opengl
- OpenGL 4.0 or greater.
- Tcl (for the in-game console, because functions don't require parentheses)
- libnoise

The code is C++ and in config.mk the default compiler is declared to be clang++

## Compilation

A simple Makefile is provided; just run make at the root level.

Options may be edited in the config.mk file or in some cases passed as
arguments.

Installation is possibly provided, but it is mostly automatic and may not work.
Install under your own responsability.

## Running

The "voxel" executable is generated in the src/ directory. by default it
creates a world in the foo/ directory from the path the executable was called.

During play, the following bindings are used:

- WASD: move the character around.
- CTRL: Run instead of walking
- Spacebar: Jump; note that the current jump is for test purposes only so
  there's no limit to the jump height.
- Mouse: rotate the character
- Left Mouse Button: break blocks
- Right Mouse Button: place blocks
- Right Mouse Button + SHIFT: place blocks sideways
- 1 through 9: select hotbar slot

Controls behave differently when the mouse points to the top side of a crate or
board:

- Left Mouse Button: Move all items from the current hotbar slot to the pointed
  block inventory slot.
- Right Mouse Button: Move one item from the current hotbar slot to the pointed
  block inventory slot.
- Left Mouse Button + SHIFT: Move all items from the pointed block inventory
  slot to the current hotbar slot.
- Right mouse Button + SHIFT: Move one items from the pointed block inventory
  slot to the current hotbar slot.

Benches behave similarly to crates, but when the cursor points to a particular
inventory slot, a second hotbar appears displaying all possible results of the
recipes involving the object in the pointed slot. This includes the object on
the slot itself in the first, so by default benches behave like crates, but:

- Mouse wheel + SHIFT: Scroll through the secondary hotbar holding all valid
  recipes that involve the object in the slot pointed at by the cursor.

When pressing a Mouse Button + SHIFT, the corresponding amount of items frm the
secondary hotbar will be moved to the player's hotbar, and objects will be
removed from the bench according to the recipe.

Crates have sixteen inventory spaces, benches have nine, and boards have
sixty-four; boards won't take more than one item in each slot, though.

- The period (.) key allows entering commands

## Commands

The current application takes commands in Tcl. It may be a weird language, but
it is a weird language that doesn't need extra characters such as parenthesis
or commas for simple commands.

In addition to the usual Tcl commands, the application accepts the following
ones:

`tp <x> <y> <z>`:
Teleport to the given position

`give <item> [<amount>]`:
add amount (default: 1) of item to player's inventory

`take <item> [<amount>]`:
remove amount (default: 1) of item from player's inventory

`take all`:
empty player's inventory

`query`:
print data about the block you're looking at

`a`:
set the block you're looking at as first selection corner

`b`:
set the block you're looking at as second selection corner

`box <block>`:
fill the box defined by the "a" and "b" commands with "block"

`hbox <block>`:
fill the outer shell of the box defined by the "a" and "b" commands with
"block"

`walls <block>`:
fill the walls of the box defined by the "a" and "b" commands with "block"

`replace <from-block> <to-block>`:
replace "from-block" with "to-block" in the box defined with the "a" and "b"
commands

`relit`:
Recalculate lighting in the box defined with the "a" and "b" commands

`regen`:
Regenerate all terrain in the chunk the player is standing on.

## Item and Block Names

Items are objects that can occuppy an inventory slot, be it of the player or a
block with an inventory, or exist as a drop entity.

Items are built by combining a material with an object. The name is built by
concatenating the material and the object with a underscore character (`_`)

Blocks are objects that occupy a position in the world; they can be built by
placing items, and they may drop items when destroyed, but are not the same.

Blocks are built by combining a material with a shape. The name is built by
concatenating the material and the shape with a underscore character (`_`)

For both items and blocks, the name may be built without specifying an object
or shape, in that case the `block` object and `block_dn` shape will be used.

Finally, an "special" block name is "air"; it generates a block with no shape
and undetermined material.

### Materials

dirt grass limestone limestone_cobble limestone_lbrick limestone_sbrick
limestone_tile limestone_column limestone_block limestone_sand limestone_crate
limestone_bench limestone_pipe limestone_board sandstone sandstone_cobble
sandstone_lbrick sandstone_sbrick sandstone_tile sandstone_column
sandstone_block sandstone_sand sandstone_crate sandstone_bench sandstone_pipe
sandstone_board marble marble_cobble marble_lbrick marble_sbrick marble_tile
marble_column marble_block marble_sand marble_crate marble_bench marble_pipe
marble_board granite granite_cobble granite_lbrick granite_sbrick granite_tile
granite_column granite_block granite_sand granite_crate granite_bench
granite_pipe granite_board basalt basalt_cobble basalt_lbrick basalt_sbrick
basalt_tile basalt_column basalt_block basalt_sand basalt_crate basalt_bench
basalt_pipe basalt_board slate slate_cobble slate_lbrick slate_sbrick
slate_tile slate_column slate_block slate_sand slate_crate slate_bench
slate_pipe slate_board clay clay_cobble clay_lbrick clay_sbrick clay_tile
clay_column clay_block clay_sand clay_crate clay_bench clay_pipe clat_board
wood1 wood1_crate wood1_bench wood1_pipe wood1_log wood1_board wood2
wood2_crate wood2_bench wood2_pipe wood2_log wood2_board wood3 wood3_crate
wood3_bench wood3_pipe wood3_log wood3_board wood4 wood4_crate wood4_bench
wood4_pipe wood4_log wood4_board wood5 wood5_crate wood5_bench wood5_pipe
wood5_log wood5_board wood6 wood6_crate wood6_bench wood6_pipe wood6_log
wood6_board wood7 wood7_crate wood7_bench wood7_pipe wood7_log wood7_board
rune_o rune_k rune_a rune_n rune_j rune_c rune_e

### Objects

block slab stairs pane fluid token_lf token_bk token_rt token_ft

### Shapes

none block_dn block_up block_lf block_rt block_bk block_ft slab_dn slab_up
slab_lf slab_rt slab_bk slab_ft pane_x pane_y pane_z stairs_df stairs_dl
stairs_db stairs_dr stairs_uf stairs_ul stairs_ub stairs_ur pane_up pane_dn
pane_lf pane_rt pane_bk pane_ft

Item names may be used with the `give` and `take` commands:

	give limestone
	give limestone_lbrick_stairs 64
	give wood1_bench

Block names may be used with commands such as `box` or `replace`:

	box limestone_block_up
	replace limestone_lbrick_stairs_ul air

## Recipes

In a bench, objects can be made into other objects; these are the possible
combinations:

- Any three blocks in a row >> 6 slabs
- Any three slabs in a row >> 6 panes
- Any six blocks in a stair shape >> 6 stairs
- Single stone block >> 1 large brick block of the same base material
- Single large brick block >> 1 small brick block of the same material
- Single small brick block >> 1 tile block of the same material
- Single wooden log >> 4 wood blocks of the same base material
- Four panes in diamond shape >> 1 pipe of the same material
- Five panes in cross shape >> 1 crate of the same material
- Four blocks in square shape >> 1 bench of the same material

Note that at the current moment not all blocks are obtainable through recipes
and some are not supposed to be.
