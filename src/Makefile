
include ../config.mk

# Independently of where the files being compiled are, they search for C
# headers in the current directory.
ALL_CXXFLAGS += -I$(shell pwd)/..

voxel_gens += v2.h
voxel_gens += v3.h
voxel_gens += v4.h
voxel_gens += box2.h
voxel_gens += box3.h

voxel_objs += camera.o
voxel_objs += shard.o
voxel_objs += chunk.o
voxel_objs += world.o
voxel_objs += command_line.o
voxel_objs += context.o
voxel_objs += fps_manager.o
#voxel_objs += flowsim.o
voxel_objs += inventory.o
voxel_objs += lighting.o
voxel_objs += main_loop.o
voxel_objs += media.o
voxel_objs += option_parser.o
voxel_objs += physics.o
#voxel_objs += profile.o
voxel_objs += recipes.o
voxel_objs += renderer.o
voxel_objs += vertex_buffer.o
voxel_objs += shader.o
voxel_objs += serializer.o
voxel_objs += tcl_commands.o
voxel_objs += terraform.o
voxel_objs += tone_mapper.o
voxel_objs += types.o
voxel_objs += entity.o
voxel_objs += roaming_entity.o
voxel_objs += bench_entity.o
voxel_objs += board_entity.o
voxel_objs += crate_entity.o
voxel_objs += pipe_entity.o
voxel_objs += drop_entity.o
voxel_objs += player_entity.o
voxel_objs += noiseutils.o

szcat_objs += serializer.o

gens = $(voxel_gens)
objs = $(voxel_objs) $(szcat_objs)
deps = $(objs:.o=.d)

programs += voxel
programs += szcat

install_programs = $(addprefix $(DESTDIR)$(bindir)/,$(programs))

files += data

install_files = $(addprefix $(DESTDIR)$(datadir)/,$(files))

# The default target of this makefile
.PHONY: all
all: $(gens) $(programs)

.PHONY: clean
clean:
	$(RM) $(gens)
	$(RM) $(deps)
	$(RM) $(objs)
	$(RM) $(programs)

.PHONY: install
install: all $(install_programs) $(install_files) $(DESTDIR)$(localstatedir)

$(DESTDIR)$(localstatedir):
	$(QUIET_INSTALL)$(INSTALL) -d $@ -g ftu -m 0770

# Rules to compile tools that require specific libraries or flags
voxel: $(voxel_objs)
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) $(ALL_LDFLAGS) -o $@ $^ -lGL -lGLU -lSDL2 -lSDL2_mixer -lSDL2_image -lnoise

szcat: $(szcat_objs)

-include $(deps)

