#!/bin/bash

# 1. Check if a filename was provided
if [ -z "$1" ]; then
    echo "Usage: ./compile.sh <path/to/filename.c>"
    exit 1
fi

SOURCE_FILE="$1"

# Extract directory and base filename
DIR_NAME=$(dirname "$SOURCE_FILE")
BASE_NAME=$(basename "$SOURCE_FILE" .c)
INCLUDE_DIRS="-Ilib -Iprograms"

# Construct full paths for output files
OBJ_FILE="${DIR_NAME}/${BASE_NAME}.o"
ELF_FILE="${DIR_NAME}/${BASE_NAME}.elf"

# Lib related
LIB_DIR="lib"
START_SRC="${LIB_DIR}/start.S"
START_OBJ="${LIB_DIR}/start.o"
SYSCALL_SRC="${LIB_DIR}/syscall.c"
SYSCALL_OBJ="${LIB_DIR}/syscall.o"
STDIO_SRC="${LIB_DIR}/stdio.c"
STDIO_OBJ="${LIB_DIR}/stdio.o"
STDLIB_SRC="${LIB_DIR}/stdlib.c"
STDLIB_OBJ="${LIB_DIR}/stdlib.o"

# Toolchain Variables
CC=riscv64-unknown-elf-gcc
LD=riscv64-unknown-elf-ld

# Flags
CFLAGS="-march=rv32im -mabi=ilp32 -c -nostdlib -fno-builtin -mno-relax"
LDFLAGS="-m elf32lriscv -T scripts/linker.ld -nostdlib --no-warn-rwx-segments"

echo "--- Building $SOURCE_FILE in directory: $DIR_NAME ---"
# Step 1: Compile Helpers (start.S, syscalls.c, stdio.c, stdlib.c)
if [ ! -f "$START_OBJ" ] || [ ! -f "$SYSCALL_OBJ" ] || [ ! -f "$STDIO_OBJ" ] || [ ! -f "$STDLIB_OBJ" ]; then
    echo "[1/4] Compiling System Libs..."
    $CC $CFLAGS $INCLUDE_DIRS "$START_SRC" -o "$START_OBJ"
    $CC $CFLAGS $INCLUDE_DIRS "$SYSCALL_SRC" -o "$SYSCALL_OBJ"
    $CC $CFLAGS $INCLUDE_DIRS "$STDIO_SRC" -o "$STDIO_OBJ"
    $CC $CFLAGS $INCLUDE_DIRS "$STDLIB_SRC" -o "$STDLIB_OBJ"
else
    echo "[1/4] System Libs already compiled."
fi


# Step 2: Compile to Object file
echo "[2/4] Compiling..."
$CC $CFLAGS $INCLUDE_DIRS "$SOURCE_FILE" -o "$OBJ_FILE"
if [ $? -ne 0 ]; then echo "Error: Compilation failed"; exit 1; fi

# Step 3: Link to ELF
echo "[3/5] Linking..."
$LD $LDFLAGS "$START_OBJ" "$SYSCALL_OBJ" "$STDIO_OBJ" "$STDLIB_OBJ" "$OBJ_FILE" -o "$ELF_FILE"
if [ $? -ne 0 ]; then echo "Error: Linking failed"; exit 1; fi


# Step 4: Cleanup
echo "[4/4] Cleaning up intermediate files..."
rm "$OBJ_FILE"

echo "--- Success! Created $BIN_FILE ---"