
TOOLCHAIN=~/bin/arm-2009q1
PREFIX=$(TOOLCHAIN)/bin/arm-none-linux-gnueabi-

AS=$(PREFIX)as
CC=$(PREFIX)gcc
CXX=$(PREFIX)g++
LD=$(PREFIX)ld

INCS=-I$(TOOLCHAIN)/arm-linux-none-gnueabi/libc/usr/include
LIBDIRS=-L$(TOOLCHAIN)/arm-linux-none-gnueabi/libc/usr/lib
LIBGCC=$(TOOLCHAIN)/lib/gcc/arm-none-linux-gnueabi/4.3.3/libgcc.a

all:obj/$(filename).S
	$(SILENT) $(CC) $(INCS) -c runtime/runtime.c -o obj/runtime.o 
	$(SILENT) $(AS) obj/$(filename).S -o obj/$(filename).o
	$(SILENT) $(CC) $(LIBDIRS) -static -lc $(LIBGCC) obj/runtime.o  obj/$(filename).o  -o bin/$(filename)

