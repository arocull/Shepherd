CC        := g++ 
LD        := g++

CCFLAGS=-g -O2 `sdl2-config --libs` -lSDL2main -lSDL2_ttf `sdl2-config --cflags` -L/usr/lib # -Wall -Werror -g
LDFLAGS=-g -O2 `sdl2-config --libs` -lSDL2main -lSDL2_ttf `sdl2-config --cflags` -L/usr/lib #-Wl,--verbose
TARGET=main

MODULES   := Core Entities Tile
SRC_DIR   := $(addprefix ./,$(MODULES)) .
BUILD_DIR := $(addprefix build/,$(MODULES))

SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ       := $(patsubst ./%.cpp,build/%.o,$(SRC))
INCLUDES  := $(addprefix -I,$(SRC_DIR))

vpath %.cpp $(SRC_DIR)

define make-goal
$1/%.o: %.cpp
	@echo "this one?"
	$(CC) $(CCFLAGS) $(INCLUDES) -c $$< -o $$@
endef

.PHONY: all checkdirs clean run debug

all: checkdirs build/$(TARGET)

build/$(TARGET): $(OBJ)
	@echo "$(SRC)"
	$(LD) $^ $(LDFLAGS) -o $@

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)

run:
	./build/main

debug:
	valgrind ./build/main

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))