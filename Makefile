# version info
MAINVERSION = 0
SUBVERSION = 2
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
	$(MAKE) -C example


.PHONY: clean
clean:
	$(MAKE) -C src clean
	$(MAKE) -C example clean


.PHONY: install
install:
	install -d $(INCDIR)/
	install include/$(HEADER) $(INCDIR)/
	$(MAKE) -C src install


.PHONY: uninstall
uninstall:
	-rm -f $(INCDIR)/$(HEADER)
	$(MAKE) -C src uninstall
