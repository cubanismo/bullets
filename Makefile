ALIGN=q
include $(JAGSDK)/tools/build/jagdefs.mk

CFLAGS += -mshort -Wall -fno-builtin

OBJS =	startup.o \
	bullets.o \
	font.o \
	sprintf.o \
	util.o

RAW_DATA = -ii clr6x12.jft _fnt

COF = bullets.cof

PROGS = $(COF)

$(COF): $(OBJS) clr6x12.jft
	$(LINK) $(LINKFLAGS) $(OBJS) -o $@ $(RAW_DATA)

.PHONY: run
run:	$(COF)
	rjcp libre -c $(COF)

include $(JAGSDK)/tools/build/jagrules.mk
