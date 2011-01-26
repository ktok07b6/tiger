TOP=../..
include $(TOP)/common.mk

DEFS+=-DDEBUG
VPATH=$(TOP) $(SUB_DIRS)

LDFLAGS=-L/usr/local/lib -L$(TOP)
LIBS=-lstdc++ -lm -lc

SRC_CANON=CanonTest.cpp
OBJ_CANON=$(call objs_from_sources, $(SRC_CANON))

all: do_all_test


do_all_test: CanonTest
	./CanonTest

CanonTest:$(OBJ_CANON)
	$(CXX) $(LDFLAGS) $(LIBS) $(OBJ_CANON) -lcore -o$@

clean:
	@rm $(OBJ_CANON)
	@rm CanonTest
