#default target
TARGET=x86

ifeq ($(TARGET), x86)
INCS+=-I../gc/boehmgc/include
LIBDIRS+=-L../gc/boehmgc/lib/$(TARGET)
LIBS=-lgc
OPTS=-m32 -g
ASMOPTS=-32
endif
ifeq ($(TARGET), arm)
TOOLCHAIN=~/bin/arm-2009q1
PREFIX=$(TOOLCHAIN)/bin/arm-none-linux-gnueabi-

INCS=-I$(TOOLCHAIN)/arm-linux-none-gnueabi/libc/usr/include
LIBDIRS=-L$(TOOLCHAIN)/arm-linux-none-gnueabi/libc/usr/lib
LIBGCC=$(TOOLCHAIN)/lib/gcc/arm-none-linux-gnueabi/4.3.3/libgcc.a
endif


AS=$(PREFIX)as
CC=$(PREFIX)gcc
CXX=$(PREFIX)g++
LD=$(PREFIX)ld

all:obj/$(filename).S
	$(SILENT) $(CC) $(OPTS) $(INCS) -c runtime/runtime.c -o obj/runtime.o 
	$(SILENT) $(AS) $(ASMOPTS) obj/$(filename).S -o obj/$(filename).o
	$(SILENT) $(CC) $(OPTS) $(LIBDIRS) -static -lc $(LIBGCC) obj/runtime.o  obj/$(filename).o $(LIBS) -o bin/$(filename)

