# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Icore/machine -Icore/common -Icore/kernel

# Target
TARGET := rv32sysemu

# Directories
OBJ_DIR := build
MACHINE_DIR := core/machine
KERNEL_DIR := core/kernel

# Programs
PROGRAMS_DIR := programs
PROGRAM_SOURCES := $(wildcard $(PROGRAMS_DIR)/*.c)

# Sources and objects
SRCS := core/main.cpp $(wildcard $(MACHINE_DIR)/*.cpp ) $(wildcard $(KERNEL_DIR)/*.cpp)
OBJS := $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@

# Compile
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

all: $(TARGET) programs

# Clean
clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	rm -f programs/*.bin
	rm -f programs/*.o
	rm lib/*.o

programs:
	@for src in $(PROGRAM_SOURCES); do \
		echo "[PROGRAM] Compiling $$src"; \
		scripts/compile.sh $$src; \
	done

.PHONY: all clean programs