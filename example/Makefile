CROSS_COMPILE ?=

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar


OBJECTS = myprog.o
TARGET = myprog
CFLAGS += -I../include -O2 -fPIC
ifeq ($(CONFIG_SUPPORT_LUA), 1)
LDLIBS += -llua -lm
endif

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS) ../src/libklogging.a
	$(LD) -o $@ $(LDFLAGS) $^ $(LDLIBS) -lstdc++

%.o: %.c
	$(CC) $(CFLAGS) -c $<

-include $(OBJECTS:.o=.d)

%.d: %.c
	@set -e; \
	rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


.PHONY: clean
clean:
	-rm -f $(OBJECTS) $(OBJECTS:.o=.d) $(TARGET)
