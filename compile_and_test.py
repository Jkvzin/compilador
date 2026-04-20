#!/usr/bin/env python3
import subprocess
import os
import sys

os.chdir(r"C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop")

# Compile
print("=" * 60)
print("Compiling parser with tests...")
print("=" * 60)

cmd = [
    "gcc",
    "-Wall", "-Wextra", "-std=c99", "-g", "-fPIC",
    "src\\ast.c", "src\\lexer.c", "src\\parser.c", "src\\token.c", "src\\symbol.c",
    "tests\\test_parser.c",
    "-lm",
    "-o", "test_parser.exe"
]

result = subprocess.run(cmd, capture_output=True, text=True)

if result.returncode != 0:
    print("COMPILATION FAILED!")
    print(result.stdout)
    print(result.stderr)
    sys.exit(1)

print("\nCompilation successful!")
print("\n" + "=" * 60)
print("Running tests...")
print("=" * 60 + "\n")

# Run tests
result = subprocess.run(["test_parser.exe"], capture_output=True, text=True)
print(result.stdout)
if result.stderr:
    print("STDERR:", result.stderr)

sys.exit(result.returncode)
