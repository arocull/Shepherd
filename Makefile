# Makefile created by Alan O'Cull using various resources off the internet
# Here is one of them:
# https://cs.colby.edu/maxwell/courses/tutorials/maketutor/

# Compilation command
CC=g++
# gcc/g++ Compilation Flags
# 	-g[1-3] for debugging symbols, -O[1-3] for optimization levels
# 	-Wall for warnings, -Werror for warnings as errors 
CFLAGS= -g -O2 `sdl2-config --libs` -lSDL2main -lSDL2_mixer `sdl2-config --cflags` -L/usr/lib

# Build files should go in this directory
ODIR = build
# Save files should go in this directory
SDIR = save

# For modules, list any and all subfolders that need to be compiled
MODULES := Audio Map Triggers Entities Entities/AI Entities/Subclasses Entities/Boss Core Core/Input Core/UI
# Apply 'src/' to all modules/subfolders
SRC_DIR  := . $(MODULES)
INCLUDES := $(addprefix -I, $(SRC_DIR))
# Same as above, but utilizes build directory
BUILD_DIR := $(addprefix $(ODIR)/, $(SRC_DIR))
# Auto grab header file dependencies
DEPS := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.h))
# Grab corresponding object files that will be compiled
OBJS := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))

# Each CPP file needs a corresponding object output
_OBJ := $(patsubst %.cpp, %.o, $(OBJS)) Core/main.o
# Objects should be outputted in build directory
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

.PHONY: all checkdirs clean run runc
all: checkdirs build # Set default functionality
runc: clean checkdirs build run

## DEFAULT BEHAVIOR
# Create necessary directories
$(BUILD_DIR):
	@mkdir -p $@

# Then perform compilation
$(ODIR)/%.o: %.cpp $(SRC_DIR)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDES)

# Finally build target / goal
build: $(OBJ)
	$(CC) $^ $(CFLAGS) -o build/Core/main

checkdirs: $(BUILD_DIR) # Create necessary directories
	@mkdir -p $(SDIR)

clean: # Delete build directory
	rm -rf $(ODIR)
	rm -rf $(SDIR)

run: # Run code
	./build/Core/main

debug: # Run code with gdb
	gdb ./build/Core/main

debug-mem: # Run code with valgrind
	valgrind ./build/Core/main