
include config.mk

# Independently of where the files being compiled are, they search for C
# headers in the current directory.
ALL_CFLAGS += -I$(shell pwd)

voxel_objs += chunk.o
voxel_objs += simplex.o
voxel_objs += sz.o
voxel_objs += terraform.o
voxel_objs += voxel.o

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

szcat: $(szcat_objs)

-include $(DEPS)

