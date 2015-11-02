# Default values
TYPE = debug
CXX = g++

# Load project settings
CONFIG = default
include make/$(CONFIG)

# Set compile flags for debug build
ifeq ($(TYPE),debug)
COMPILE_FLAGS += -g -DDEBUG_MODE=true
endif

# Set compile flags for release build
ifeq ($(TYPE),release)
COMPILE_FLAGS += -O3
endif

NAME = bin/$(CONFIG)/$(TYPE)/$(PROGRAM_NAME)
ifneq ($(LIBS_EXTERN), )
LIBS = $(shell pkg-config --libs --cflags $(LIBS_EXTERN))
endif
COMPILE_FLAGS += -Wno-write-strings -std=c++14 -Wall -Wextra $(COMPILE_FLAGS_EXTRA) $(DEFS) -Winvalid-pch
# TODO: Make this automated
INCLUDES = -I build/header/$(CONFIG)/$(TYPE) -I include -I ../include
STATIC_LIBS = ../bin/$(CONFIG)/$(TYPE)/flare.a
# MAKEFLAGS = "-j 4"

SOURCES = $(shell find src -name '*.cpp' -printf '%T@\t%p\n' | sort -k 1nr | cut -f2-)
SOURCES_LIBS = $(shell find libs/src -name '*.cpp' -printf '%T@\t%p\n' | sort -k 1nr | cut -f2-)

HEADERS = $(shell find include -name '*.h' -printf '%T@\t%p\n' | sort -k 1nr | cut -f2-)
HEADERS_LIBS = $(shell find libs/include -name '*.h' -printf '%T@\t%p\n' | sort -k 1nr | cut -f2-)

OBJECTS = $(subst .cpp,.o,$(subst src/,build/obj/$(CONFIG)/$(TYPE)/, $(SOURCES)))
OBJECTS_LIBS = $(subst .cpp,.o,$(subst libs/src/,build/obj/$(CONFIG)/$(TYPE)/libs/, $(SOURCES_LIBS)))

FOLDERS = $(subst src,build/obj/$(CONFIG)/$(TYPE),$(shell find src -type d | sort -k 1nr | cut -f2-))
FOLDERS_LIBS = $(subst libs/src,build/obj/$(CONFIG)/$(TYPE)/libs,$(shell find libs/src -type d | sort -k 1nr | cut -f2-))

TOTAL = $(words $(SOURCES) $(SOURCES_LIBS))
COUNTER = 1

all: dirs build/header/$(CONFIG)/$(TYPE)/standard.h.gch $(NAME)

dirs:
ifneq ($(wildcard $(FOLDERS) $(FOLDERS_LIBS) build/header/$(CONFIG)/$(TYPE) bin/$(CONFIG)/$(TYPE)),$(FOLDERS) $(FOLDERS_LIBS) build/header/$(CONFIG)/$(TYPE) bin/$(CONFIG)/$(TYPE))
	@echo "Creating folders"
	@mkdir -p $(FOLDERS)
	@mkdir -p $(FOLDERS_LIBS)
	@mkdir -p build/header/$(CONFIG)/$(TYPE)
	@mkdir -p bin/$(CONFIG)/$(TYPE)
endif

$(NAME): $(OBJECTS_LIBS) $(OBJECTS) ../bin/$(CONFIG)/$(TYPE)/flare.a
	@echo "Linking: $(NAME)"
	@$(CXX) $(INCLUDES) $(LIBS) $(DEFS) $(COMPILE_FLAGS) -o $@ $^ $(STATIC_LIBS) 

$(OBJECTS) : build/obj/$(CONFIG)/$(TYPE)/%.o : src/%.cpp build/header/$(CONFIG)/$(TYPE)/standard.h.gch
	@echo "Compiling: $<"
	@$(CXX) $(INCLUDES) $(LIBS) $(DEFS) $(COMPILE_FLAGS) -c -o $@ $<

$(OBJECTS_LIBS) : build/obj/$(CONFIG)/$(TYPE)/libs/%.o : libs/src/%.cpp ../build/header/$(CONFIG)/$(TYPE)/flare.h.gch
	@echo "Compiling: $<"
	@$(CXX) $(INCLUDES) $(LIBS) $(DEFS) $(COMPILE_FLAGS) -c -o $@ $< -Wno-type-limits -Wno-missing-field-initializers

build/header/$(CONFIG)/$(TYPE)/standard.h.gch: $(HEADERS) $(HEADERS_LIBS) ../build/header/$(CONFIG)/$(TYPE)/flare.h.gch
	@echo "Compiling: include/standard.h"
	@$(CXX) $(COMPILE_FLAGS) $(INCLUDES) include/standard.h -o build/header/$(CONFIG)/$(TYPE)/standard.h.gch

run:
	@echo "Running: $(NAME)"
	@cd sandbox && ./../$(NAME)

debug:
	@echo "Running: $(NAME) (using gdb)"
	@cd sandbox && gdb ../$(NAME) -ex run -ex bt -ex quit --silent

valgrind:
	@echo "Running: $(NAME) (using valgrind)"
	@cd sandbox && valgrind ./../$(NAME) -ex run -ex bt -ex quit --silent

clean:
	@echo "Removing build files"
	@rm -rf build/
	@rm -rf bin/
	@rm -f include/standard.h.gch
