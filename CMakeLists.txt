cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Project Name
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
project(rv32kernel CXX)

# --- 1. Compiler Settings ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add warnings (equivalent to -Wall -Wextra)
add_compile_options(-Wall -Wextra)

# --- 2. Include Directories ---
# Equivalent to -Icore/machine -Icore/common -Icore/kernel
include_directories(
    core/machine
    core/common
    core/kernel
)

# --- 3. Source Files ---
# Find sources (Equivalent to wildcard)
file(GLOB_RECURSE SOURCES 
    "core/main.cpp"
    "core/machine/*.cpp" 
    "core/kernel/*.cpp"
)

# --- 4. Main Target (The Emulator) ---
add_executable(rv32kernel ${SOURCES})

# --- 5. Custom Target: Programs ---
# Find all .c files in programs/
file(GLOB PROGRAM_SOURCES "programs/*.c")

# List to keep track of generated bin files
set(PROGRAM_BINS "")

foreach(SOURCE_FILE ${PROGRAM_SOURCES})
    # Get filename without extension (e.g., "programs/test.c" -> "test")
    get_filename_component(NAME_WE ${SOURCE_FILE} NAME_WE)
    
    # Define expected output file (assuming your script creates .bin files in the same dir)
    set(OUTPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/programs/${NAME_WE}.bin")
    
    # Create a rule to generate the .bin from the .c
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND ${CMAKE_SOURCE_DIR}/scripts/compile.sh ${SOURCE_FILE}
        DEPENDS ${SOURCE_FILE}
        COMMENT "[PROGRAM] Compiling ${NAME_WE}.c"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    
    list(APPEND PROGRAM_BINS ${OUTPUT_FILE})
endforeach()

# Create a target 'programs' that depends on all the .bin files
# 'ALL' ensures this runs by default when you type 'make'
add_custom_target(programs ALL DEPENDS ${PROGRAM_BINS})

# Helper to clean up the generated .bin files when running 'make clean'
set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES "${PROGRAM_BINS}")