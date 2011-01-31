BUILD_DIR=$(TOP)/obj
TARGET_CPU=arm
SUB_DIRS=asm ast canon common frame graph opt parse regalloc symbol temp translate tree type target/$(TARGET_CPU)
INCS=-I$/usr/local/include -I$(HOME)/include -I$(TOP) $(addprefix -I$(TOP)/, $(SUB_DIRS))
OPTS=-fno-exceptions -fno-rtti -O0 -g -Wall 
CFLAGS=$(INCS) $(OPTS) $(DEFS)
CXXFLAGS=$(CFLAGS)

CC=$(PREFIX)gcc
CXX=$(PREFIX)g++
AS=$(CC)
AR=$(PREFIX)ar

COMPILE.c.cmdline = $(CC) -c $(CFLAGS) -o $(1) $<
COMPILE.cpp.cmdline = $(CXX) -c  $(CXXFLAGS) -o $(1) $(2)
GENDEP.c.cmdline = $(CC) -c $(CFLAGS) -MM  -MF $(1) -MT "$(patsubst %.dep,%.o, $(1)) $(1)" $(2)
GENDEP.cpp.cmdline = $(CC) -c $(CXXFLAGS) -MM  -MF $(1) -MT "$(patsubst %.dep,%.o, $(1)) $(1)" $(2)
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
