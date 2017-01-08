
include ../config.mk

# Independently of where the files being compiled are, they search for C
# headers in the current directory.
ALL_CFLAGS += -I$(shell pwd)

voxel_objs += camera.o
voxel_objs += chunk.o
voxel_objs += context.o
voxel_objs += drop.o
voxel_objs += fps_manager.o
voxel_objs += inventory.o
voxel_objs += lighting.o
voxel_objs += main_loop.o
voxel_objs += media.o
voxel_objs += physics.o
voxel_objs += profile.o
voxel_objs += simplex.o
voxel_objs += recipes.o
voxel_objs += renderer.o
voxel_objs += roam.o
voxel_objs += shader.o
voxel_objs += sz.o
voxel_objs += terraform.o
voxel_objs += tone_mapper.o

szcat_objs += sz.o

objs = $(voxel_objs) $(szcat_objs)
deps = $(objs:.o=.d)

programs += voxel
programs += szcat

install_programs = $(addprefix $(DESTDIR)$(bindir)/,$(programs))

files += data

install_files = $(addprefix $(DESTDIR)$(datadir)/,$(files))

# The default target of this makefile
.PHONY: all
all: $(programs)

.PHONY: clean
clean:
	$(RM) $(deps)
	$(RM) $(objs)
	$(RM) $(programs)

.PHONY: install
install: all $(install_programs) $(install_files) $(DESTDIR)$(localstatedir)

$(DESTDIR)$(localstatedir):
	$(QUIET_INSTALL)$(INSTALL) -d $@ -g ftu -m 0770

# Rules to compile tools that require specific libraries or flags
voxel: $(voxel_objs)
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) $(ALL_LDFLAGS) -o $@ $^ -lGL -lGLU -lSDL2 -lSDL2_mixer -lSDL2_image

szcat: $(szcat_objs)

-include $(deps)

