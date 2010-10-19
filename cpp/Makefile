BUILD_DIR=obj
ifdef YDEBUG
DEFS=-DYYERROR_VERBOSE -DYYDEBUG
endif
DEFS+=-DDEBUG
OPTS=-O0 -g -Wall
INCS=-I/usr/local/include -I./ -Iast -Icanon -Icommon -Iframe -Iparse -Isymbol -Itarget/arm -Itemp -Itranslate -Itree -Itype

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

VPATH=./ ast canon common frame parse symbol target/arm temp translate tree type

SOURCES_CPP= \
	Object.cpp \
	lexer.cpp \
	parser.cpp \
	tiger.cpp \
	Symbol.cpp \
	Type.cpp \
	AbsynPrinter.cpp \
	AST2DotTranslater.cpp \
	TypeCheck.cpp \
	Temp.cpp \
	Label.cpp \
	Level.cpp \
	Frame.cpp \
	ARMFrame.cpp \
	Tree.cpp \
	IRTranslater.cpp \
	TreePrinter.cpp \
	Tree2DotTranslater.cpp \
	main.cpp \
	Ex.cpp Cx.cpp Nx.cpp\
	HeapManager.cpp \
	Canon.cpp \
	BasicBlocks.cpp \
	Trace.cpp \
	$(NULL)

SOURCES_C= \
	$(NULL)

LDFLAGS=-L/usr/local/lib
LIBS=-lstdc++  -lm -lc 
TARGET=tiger

OBJECTS=$(call objs_from_sources, $(SOURCES_CPP))
OBJECTS+=$(call objs_from_sources, $(SOURCES_C))
DEPS=$(call deps_from_sources, $(SOURCES_CPP))

all: $(TARGET)

tiger: $(DEPS) $(OBJECTS) 
	$(CXX) $(LDFLAGS) $(LIBS) $(OBJECTS) -o tiger

parse/parser.cpp: parse/tiger.y
	yacc -v -oparse/parser.cpp -d parse/tiger.y
	-@mv parser.cpp.h parser.hpp

parse/lexer.cpp: parse/tiger.l parse/parser.cpp
	flex -oparse/lexer.cpp parse/tiger.l

clean:
	-@echo -@rm $(OBJECTS) $(TARGET)
	-@rm $(OBJECTS) $(TARGET) 
	-@rm lexer.* parser.*
