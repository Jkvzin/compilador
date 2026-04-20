#!/usr/bin/env python3
import os
import subprocess
import sys

# Change to project directory
project_dir = r"C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"
os.chdir(project_dir)

# Create .obj directory if it doesn't exist
obj_dir = ".obj"
if not os.path.exists(obj_dir):
    os.makedirs(obj_dir)
    print(f"Created {obj_dir} directory")

# Compilation settings
cc = "gcc"
cflags = ["-Wall", "-Wextra", "-std=c99", "-g"]
ldflags = ["-lm"]

# Source files to compile
sources = [
    ("src/token.c", ".obj/token.o"),
    ("src/lexer.c", ".obj/lexer.o"),
    ("src/ast.c", ".obj/ast.o"),
    ("src/symbol.c", ".obj/symbol.o"),
    ("src/parser.c", ".obj/parser.o"),
    ("tests/test_parser.c", ".obj/test_parser.o"),
]

# Object files for linking
obj_files = [obj for src, obj in sources]

print("=" * 60)
print("COMPILING SIMPLES PARSER")
print("=" * 60)

# Compile each source file
for src, obj in sources:
    print(f"\nCompiling {src}...")
    cmd = [cc] + cflags + ["-c", src, "-o", obj]
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    if result.returncode != 0:
        print(f"ERROR: Failed to compile {src}")
        print(result.stderr)
        sys.exit(1)
    else:
        print(f"✓ {obj} created")

# Link the executable
print(f"\n{'=' * 60}")
print("LINKING EXECUTABLE")
print(f"{'=' * 60}\n")

executable = "test_parser.exe"
cmd = [cc] + cflags + obj_files + ["-o", executable] + ldflags
result = subprocess.run(cmd, capture_output=True, text=True)

if result.returncode != 0:
    print(f"ERROR: Failed to link {executable}")
    print(result.stderr)
    sys.exit(1)
else:
    print(f"✓ {executable} created successfully")

# Run the executable
print(f"\n{'=' * 60}")
print("RUNNING TESTS")
print(f"{'=' * 60}\n")

result = subprocess.run([f"./{executable}"], capture_output=False, text=True)
sys.exit(result.returncode)
