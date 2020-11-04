# version info
MAINVERSION = 0
SUBVERSION = 1
PATCHLEVEL =

export VERSION = $(MAINVERSION).$(SUBVERSION)$(if $(PATCHLEVEL),.$(PATCHLEVEL))

# compile
export DESTDIR ?=
export PREFIX ?= /usr/local
export INCDIR ?= $(DESTDIR)$(PREFIX)/include
export LIBDIR ?= $(DESTDIR)$(PREFIX)/lib
export BINDIR ?= $(DESTDIR)$(PREFIX)/bin

HEADER = klogging.h

.PHONY: all
all:
	$(MAKE) -C src


.PHONY: clean
clean:
	$(MAKE) -C src clean


.PHONY: install
install:
	install -d $(INCDIR)/
	install include/$(HEADER) $(INCDIR)/
	$(MAKE) -C src install


.PHONY: uninstall
uninstall:
	-rm -f $(INCDIR)/$(HEADER)
	$(MAKE) -C src uninstall
