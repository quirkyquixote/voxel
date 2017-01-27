Yet Another MineCraft-ish Engine
================================

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

Requirements
------------

The following libraries are needed to compile this:

- SDL2, including SDL_image, SDL_mixer, and SDL_opengl
- OpenGL 4.0 or greater.
- Tcl (for the in-game console, because functions don't require parentheses)
- libnoise

The code is C++ and in config.mk the default compiler is declared to be clang++

Compilation
-----------

A simple Makefile is provided; just run make at the root level.

Options may be edited in the config.mk file or in some cases passed as
arguments.

Installation is possibly provided, but it is mostly automatic and may not work.
Install under your own responsability.

Running
-------

The "voxel" executable is generated in the src/ directory. by default it
creates a world in the foo/ directory from the path the executable was called.

