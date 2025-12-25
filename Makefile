# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude

# Target
TARGET := rv32emu

# Directories
OBJ_DIR := build
MACHINE_DIR := core/machine


# Sources and objects
SRCS := core/main.cpp $(wildcard $(MACHINE_DIR)/*.cpp)
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

# Clean
clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	rm -f examples/*.bin
	
.PHONY: all clean