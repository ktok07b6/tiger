
PREFIX=/opt/arm-2009q1/bin/arm-none-linux-gnueabi-

AS=$(PREFIX)as
CC=$(PREFIX)gcc
CXX=$(PREFIX)g++
LD=$(PREFIX)ld

INCS=-I/opt/arm-2009q1/arm-none-linux-gnueabi/libc/usr/include
LIBDIRS=-L/opt/arm-2009q1/arm-none-linux-gnueabi/libc/usr/lib
LIBGCC=/opt/arm-2009q1/lib/gcc/arm-none-linux-gnueabi/4.3.3/libgcc.a
#LIB=/opt/arm-2009q1/arm-none-linux-gnueabi/libc/usr/lib/libc.a 

#/opt/arm-2009q1/arm-none-linux-gnueabi/libc/usr/lib/crt1.o

all:tmp.s
	$(CC) $(INCS) -c runtime/runtime.c -o runtime.o
	$(AS) $(filename).S -o $(filename).o
	$(CC) $(LIBDIRS) -static -lc $(LIBGCC) runtime.o  $(filename).o  -o $(filename)

