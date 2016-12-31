
# Where this file resides

root_dir := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Retrieve version numbers from git repo

VERSION = $(shell git describe)

# Determine where we are going to install things

prefix = /usr/local
suffix = $(shell echo $(VERSION) | grep -o '^[0-9]*\.[0-9]*')
bindir = $(prefix)/bin
libdir = $(prefix)/lib
datadir = $(prefix)/share/tnt$(suffix)
docdir = $(prefix)/share/doc/tnt$(suffix)
htmldir = $(docdir)
pdfdir = $(docdir)
localstatedir = $(prefix)/var/tnt

# There are so many tools needed...

CC = cc
AR = ar
INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

# These flags may be overriden by the user

CPPFLAGS =

CFLAGS = -g \
	 -std=gnu99 \
	 -Werror \
	 -Wfatal-errors

LDFLAGS =

# These flags are necessary

ALL_CFLAGS = $(CPPFLAGS) $(CFLAGS)
ALL_LDFLAGS = $(LDFLAGS)

ALL_CFLAGS += -DVX_VERSION=\"$(VERSION)\"
ALL_CFLAGS += -DVX_LIBDIR=\"$(libdir)\"
ALL_CFLAGS += -DVX_DATADIR=\"$(datadir)\"
ALL_CFLAGS += -DVX_LOCALSTATEDIR=\"$(localstatedir)\"
ALL_CFLAGS += -MD
ALL_CFLAGS += -fPIC

ALL_LDFLAGS += -pthread

# A little function to print relevant text

QUIET_CC = @echo CC $@;
QUIET_LINK = @echo LINK $@;
QUIET_AR = @echo AR $@;
QUIET_GEN = @echo GEN $@;
QUIET_INSTALL = @echo INSTALL $@;

# Some generic targets that are the same for all Makefiles

%: %.c 

%: %.o
	$(QUIET_LINK)$(CC) $(ALL_CFLAGS) $(ALL_LDFLAGS) -o $@ $^

%.o: %.c
	$(QUIET_CC)$(CC) $(ALL_CFLAGS) -c -o $@ $<

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
