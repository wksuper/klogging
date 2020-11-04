CROSS_COMPILE ?=

CC = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar

VPATH = ../include
OBJECTS = klogging.o
SHARED_LIB_TARGET = libklogging.so
STATIC_LIB_TARGET = libklogging.a
CFLAGS += -I$(VPATH) -fPIC -O2 -DVERSION=\"$(VERSION)\"


.PHONY: all
all: $(SHARED_LIB_TARGET) $(STATIC_LIB_TARGET)

$(SHARED_LIB_TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -shared $^ -o $@

$(STATIC_LIB_TARGET): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

-include $(OBJECTS:.o=.d)

%.d: %.cpp
	@set -e; \
	rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


.PHONY: clean
clean:
	-rm -f $(OBJECTS) $(OBJECTS:.o=.d) $(SHARED_LIB_TARGET) $(STATIC_LIB_TARGET)


.PHONY: install
install:
	install -d $(LIBDIR)/
	install $(SHARED_LIB_TARGET) $(LIBDIR)/
	install $(STATIC_LIB_TARGET) $(LIBDIR)/


.PHONY: uninstall
uninstall:
	-rm -f $(LIBDIR)/$(SHARED_LIB_TARGET)
	-rm -f $(LIBDIR)/$(STATIC_LIB_TARGET)
