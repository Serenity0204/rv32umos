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

# Construct full paths for output files
OBJ_FILE="${DIR_NAME}/${BASE_NAME}.o"
ELF_FILE="${DIR_NAME}/${BASE_NAME}.elf"
BIN_FILE="${DIR_NAME}/${BASE_NAME}.bin"

# Lib related
LIB_DIR="lib"
START_SRC="${LIB_DIR}/start.S"
SYSCALL_SRC="${LIB_DIR}/syscall.c"
START_OBJ="${LIB_DIR}/start.o"
SYSCALL_OBJ="${LIB_DIR}/syscall.o"


# Toolchain Variables
CC=riscv64-unknown-elf-gcc
LD=riscv64-unknown-elf-ld
OBJCOPY=riscv64-unknown-elf-objcopy

# Flags
CFLAGS="-march=rv32im -mabi=ilp32 -c -nostdlib -fno-builtin -mno-relax"
LDFLAGS="-m elf32lriscv -T scripts/linker.ld -nostdlib"

echo "--- Building $SOURCE_FILE in directory: $DIR_NAME ---"
# Step 1: Compile Helpers (start.S & syscalls.c)
if [ ! -f "$START_OBJ" ] || [ ! -f "$SYSCALL_OBJ" ]; then
    echo "[1/5] Compiling System Libs..."
    $CC $CFLAGS "$START_SRC" -o "$START_OBJ"
    $CC $CFLAGS "$SYSCALL_SRC" -o "$SYSCALL_OBJ"
else
    echo "[1/5] System Libs already compiled."
fi


# Step 2: Compile to Object file
echo "[2/5] Compiling..."
$CC $CFLAGS "$SOURCE_FILE" -o "$OBJ_FILE"
if [ $? -ne 0 ]; then echo "Error: Compilation failed"; exit 1; fi

# Step 3: Link
echo "[3/5] Linking..."
$LD $LDFLAGS "$START_OBJ" "$SYSCALL_OBJ" "$OBJ_FILE" -o "$ELF_FILE"
if [ $? -ne 0 ]; then echo "Error: Linking failed"; exit 1; fi

# Step 4: Convert to Binary
echo "[4/5] Generating Binary..."
$OBJCOPY -O binary "$ELF_FILE" "$BIN_FILE"
if [ $? -ne 0 ]; then echo "Error: Objcopy failed"; exit 1; fi

# Step 4: Cleanup
echo "[5/5] Cleaning up intermediate files..."
rm "$OBJ_FILE" "$ELF_FILE"

echo "--- Success! Created $BIN_FILE ---"