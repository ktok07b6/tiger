BUILD_DIR=obj
ifdef YDEBUG
DEFS=-DYYERROR_VERBOSE -DYYDEBUG
endif
DEFS+=-DDEBUG
OPTS=-O0 -g -Wall
TARGET_CPU=arm
SUB_DIRS=asm ast canon common frame graph parse regalloc symbol temp translate tree type target/$(TARGET_CPU)

INCS=-I/usr/local/include -I./ $(addprefix -I,$(SUB_DIRS))

CFLAGS=$(INCS) $(OPTS) $(DEFS)
CXXFLAGS=$(CFLAGS)

CC=$(PREFIX)gcc
CXX=$(PREFIX)g++
AS=$(CC)
AR=$(PREFIX)ar

COMPILE.c.cmdline = $(CC) -c $(CFLAGS) -o $(1) $<
COMPILE.cpp.cmdline = $(CXX) -c $(CFLAGS) $(CPPFLAGS) -o $(1) $(2)
GENDEP.c.cmdline = $(CC) -c $(CFLAGS) -MM  -MF $(1) -MT "$(patsubst %.dep,%.o, $(1)) $(1)" $(2)
GENDEP.cpp.cmdline = $(CC) -c $(CFLAGS) $(CPPFLAGS) -MM  -MF $(1) -MT "$(patsubst %.dep,%.o, $(1)) $(1)" $(2)
LINK.o.cmdline = $(CXX) $(2) $(LDFLAGS) -o $(1)
LINK.so.cmdline = $(CXX) $(2) $(LDFLAGS) -shared -o $(1)
ARCHIVE.cmdline = $(AR) $(ARFLAGS) $(1) $(2)

$(BUILD_DIR)/%.o:%.c
	@mkdir -p $(dir $@)
	$(call COMPILE.c.cmdline,$@,$<)

$(BUILD_DIR)/%.o:%.cpp
	@mkdir -p $(dir $@)
	$(call COMPILE.cpp.cmdline,$@,$<)

$(BUILD_DIR)/%.o:%.S
	@mkdir -p $(dir $@)
	$(call COMPILE.c.cmdline,$@,$<)

$(BUILD_DIR)/%.dep: %.cpp
	@mkdir -p $(dir $@)
	$(call GENDEP.cpp.cmdline,$@,$<)

$(BUILD_DIR)/%.dep: %.c
	@mkdir -p $(dir $@)
	$(call GENDEP.c.cmdline,$@,$<)

#
# Custom functions
#

define objs_from_sources
$(addprefix $(BUILD_DIR)/, \
$(patsubst %.c,%.o, $(filter %.c,$(1))) \
$(patsubst %.cpp,%.o, $(filter %.cpp,$(1))) \
$(patsubst %.S,%.o, $(filter %.S,$(1))) \
)
endef

define deps_from_sources
$(addprefix $(BUILD_DIR)/, \
$(patsubst %.c,%.dep, $(filter %.c,$(1))) \
$(patsubst %.cpp,%.dep, $(filter %.cpp,$(1))) \
)
endef

VPATH=./ $(SUB_DIRS)

SOURCES_LIBCORE= \
	Object.cpp \
	HeapManager.cpp \
	lexer.cpp \
	parser.cpp \
	tiger.cpp \
	Symbol.cpp \
	Type.cpp \
	AbsynPrinter.cpp \
	AST2DotTranslater.cpp \
	TypeCheck.cpp \
	FindEscape.cpp \
	Temp.cpp \
	Label.cpp \
	Level.cpp \
	Tree.cpp \
	IRTranslater.cpp \
	TreePrinter.cpp \
	Tree2DotTranslater.cpp \
	TreeMatcher.cpp \
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
	ARMFrame.cpp ARMCodeGen.cpp \
	$(NULL)

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
	$(AR) -q libcore.a $(OBJECTS_LIBCORE)

parse/parser.cpp: parse/tiger.y
	yacc -v -oparse/parser.cpp -d parse/tiger.y

parse/lexer.cpp: parse/tiger.l parse/parser.cpp
	flex -oparse/lexer.cpp parse/tiger.l


ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS_LIBCORE) $(DEPS_TIGER)
endif

clean:
	-@echo -@rm obj/* $(TARGET)
	-@rm obj/* $(TARGET) 
	-@rm parse/*.cpp