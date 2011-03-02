TOP=.

include common.mk

ifdef YDEBUG
DEFS=-DYYERROR_VERBOSE -DYYDEBUG
endif

DEFS+=-DDEBUG

VPATH=$(TOP) $(SUB_DIRS)

SOURCES_LIBCORE= \
	Object.cpp \
	HeapManager.cpp \
	Bitmap.cpp \
	lexer.cpp \
	parser.cpp \
	parserUtils.cpp \
	tiger.cpp \
	Symbol.cpp \
	Type.cpp \
	AbsynPrinter.cpp \
	AST2DotTranslater.cpp \
	TypeCheck.cpp \
	FindEscape.cpp \
	Temp.cpp \
	Label.cpp \
	Frame.cpp \
	Level.cpp \
	Tree.cpp \
	IRTranslater.cpp \
	TreePrinter.cpp \
	Tree2DotTranslater.cpp \
	TreeMatcher.cpp \
	TreeUtils.cpp \
	Ex.cpp Cx.cpp Nx.cpp\
	Canon.cpp \
	BasicBlocks.cpp \
	Trace.cpp \
	Instruction.cpp \
	OPER.cpp \
	MOVE.cpp \
	LABEL.cpp \
	CodeGen.cpp \
	Fragment.cpp \
	Graph.cpp \
	Node.cpp \
	FlowGraph.cpp \
	AsmFlowGraph.cpp \
	InterferenceGraph.cpp \
	Liveness.cpp \
	Color.cpp \
	RegAlloc.cpp  \
	Optimizer.cpp \
	RemoveUselessMove.cpp \
	$(NULL)

ifeq ($(TARGET_CPU), arm)
SOURCES_TARGET=ARMFrame.cpp ARMCodeGen.cpp
endif
ifeq ($(TARGET_CPU), x86)
SOURCES_TARGET=X86Frame.cpp X86CodeGen.cpp
endif

SOURCES_LIBCORE+=$(SOURCES_TARGET)

SOURCES_TIGER=main.cpp

LDFLAGS=-L/usr/local/lib -L./
LIBS=-lstdc++ -lm -lc 

OBJECTS_LIBCORE=$(call objs_from_sources, $(SOURCES_LIBCORE))
OBJECTS_TIGER=$(call objs_from_sources, $(SOURCES_TIGER))
DEPS_LIBCORE=$(call deps_from_sources, $(SOURCES_LIBCORE))
DEPS_TIGER=$(call deps_from_sources, $(SOURCES_TIGER))

all: tiger

tiger: libcore.a $(OBJECTS_TIGER) 
	$(CXX) $(LDFLAGS) $(LIBS) $(OBJECTS_TIGER) -lcore -o tiger

libcore.a: $(DEPS_LIBCORE) $(OBJECTS_LIBCORE)
	-@rm libcore.a
	$(AR) -q libcore.a $(OBJECTS_LIBCORE)

parse/parser.cpp: parse/tiger.y
	bison -v -oparse/parser.cpp -d parse/tiger.y
	-@mv parse/parser.cpp.h parse/parser.hpp

parse/lexer.cpp: parse/tiger.l parse/parser.cpp
	flex -oparse/lexer.cpp parse/tiger.l


ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS_LIBCORE) $(DEPS_TIGER)
endif

clean:
	-@echo -@rm obj/* $(TARGET) libcore.a
	-@rm obj/* $(TARGET) 
	-@rm parse/parser.cpp parse/lexer.cpp