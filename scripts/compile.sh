#!/bin/bash

# 1. Check if a filename was provided
if [ -z "$1" ]; then
    echo "Usage: ./compile.sh <path/to/filename.c>"
    exit 1
fi

SOURCE_FILE="$1"

# Extract directory and base filename
# If input is "example/test1.c":
# DIR_NAME will be "example"
# BASE_NAME will be "test1"
DIR_NAME=$(dirname "$SOURCE_FILE")
BASE_NAME=$(basename "$SOURCE_FILE" .c)

# Construct full paths for output files
OBJ_FILE="${DIR_NAME}/${BASE_NAME}.o"
ELF_FILE="${DIR_NAME}/${BASE_NAME}.elf"
BIN_FILE="${DIR_NAME}/${BASE_NAME}.bin"

# Toolchain Variables
CC=riscv64-unknown-elf-gcc
LD=riscv64-unknown-elf-ld
OBJCOPY=riscv64-unknown-elf-objcopy

# Flags
CFLAGS="-march=rv32im -mabi=ilp32 -c -nostdlib -fno-builtin"
LDFLAGS="-m elf32lriscv -Ttext 0x80000000"

echo "--- Building $SOURCE_FILE in directory: $DIR_NAME ---"

# Step 1: Compile to Object file
echo "[1/3] Compiling..."
$CC $CFLAGS "$SOURCE_FILE" -o "$OBJ_FILE"
if [ $? -ne 0 ]; then echo "Error: Compilation failed"; exit 1; fi

# Step 2: Link
echo "[2/3] Linking..."
$LD $LDFLAGS "$OBJ_FILE" -o "$ELF_FILE"
if [ $? -ne 0 ]; then echo "Error: Linking failed"; exit 1; fi

# Step 3: Convert to Binary
echo "[3/3] Generating Binary..."
$OBJCOPY -O binary "$ELF_FILE" "$BIN_FILE"
if [ $? -ne 0 ]; then echo "Error: Objcopy failed"; exit 1; fi

# Step 4: Cleanup
echo "[4/4] Cleaning up intermediate files..."
rm "$OBJ_FILE" "$ELF_FILE"

echo "--- Success! Created $BIN_FILE ---"