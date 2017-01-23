
# Where this file resides

root_dir := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Retrieve version numbers from git repo

VERSION = $(shell git describe)

# Determine where we are going to install things

prefix = /usr/local
suffix = $(shell echo $(VERSION) | grep -o '^[0-9]*\.[0-9]*')
bindir = $(prefix)/bin
libdir = $(prefix)/lib
datadir = $(prefix)/share/voxel$(suffix)
docdir = $(prefix)/share/doc/voxel$(suffix)
htmldir = $(docdir)
pdfdir = $(docdir)
localstatedir = $(prefix)/var/voxel

# There are so many tools needed...

CXX = clang++
AR = ar
SH = sh
INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

# These flags may be overriden by the user

CPPFLAGS =

CXXFLAGS = -g -std=c++11 -Werror -Wfatal-errors

LDFLAGS =

# These flags are necessary

ALL_CXXFLAGS = $(CPPFLAGS) $(CXXFLAGS)
ALL_LDFLAGS = $(LDFLAGS)

ALL_CXXFLAGS += -DVOXEL_VERSION=\"$(VERSION)\"
ALL_CXXFLAGS += -DVOXEL_LIBDIR=\"$(libdir)\"
ALL_CXXFLAGS += -DVOXEL_DATADIR=\"$(datadir)\"
ALL_CXXFLAGS += -DVOXEL_LOCALSTATEDIR=\"$(localstatedir)\"
ALL_CXXFLAGS += -MD
ALL_CXXFLAGS += -I/usr/include/tcl

ALL_LDFLAGS += -pthread
ALL_LDFLAGS += -ltcl
ALL_LDFLAGS += -lm
ALL_LDFLAGS += -lstdc++

# A little function to print relevant text

QUIET_CXX = @echo CXX $@;
QUIET_LINK = @echo LINK $@;
QUIET_AR = @echo AR $@;
QUIET_GEN = @echo GEN $@;
QUIET_INSTALL = @echo INSTALL $@;

# Some generic targets that are the same for all Makefiles

%.h: gen_%.sh
	$(QUIET_GEN)$(SH) $^ > $@

%: %.cc 

%: %.o
	$(QUIET_LINK)$(CXX) $(ALL_CXXFLAGS) $(ALL_LDFLAGS) -o $@ $^

%.o: %.cc
	$(QUIET_CXX)$(CXX) $(ALL_CXXFLAGS) -c -o $@ $<

%.a: %.o
	$(QUIET_AR)$(AR) rc $@ $^

$(DESTDIR)$(bindir)/%: %
	@$(INSTALL) -d $(@D)
	$(QUIET_INSTALL)$(INSTALL_PROGRAM) $< $@$(suffix)
	@cd $(DESTDIR)$(bindir) && ln -f -s $*$(suffix) $*

$(DESTDIR)$(libdir)/%.so: %.so
	@$(INSTALL) -d $(@D)
	$(QUIET_INSTALL)$(INSTALL_DATA) $< $@.$(VERSION)
	@cd $(DESTDIR)$(libdir) && ln -s -f $<.$(VERSION) $<

$(DESTDIR)$(datadir)/% $(DESTDIR)$(docdir)/% $(DESTDIR)$(htmldir)/% $(DESTDIR)$(pdfdir)/%: %
	@$(INSTALL) -d $(@D)
	$(QUIET_INSTALL)$(INSTALL_DATA) $< $@
