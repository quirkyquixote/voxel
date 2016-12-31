
include config.mk

# Independently of where the files being compiled are, they search for C
# headers in the current directory.
ALL_CFLAGS += -I$(shell pwd)

objs += chunk.o
objs += sz.o
objs += voxel.o

deps = $(objs:.o=.d)

programs += voxel

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
voxel: $(objs)
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) $(ALL_LDFLAGS) -o $@ $<

-include $(DEPS)

