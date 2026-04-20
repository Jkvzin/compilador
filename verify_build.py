#!/usr/bin/env python3

import subprocess
import os
import sys
import platform

# Change to project directory
proj_dir = r"C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"
os.chdir(proj_dir)

print("=" * 70)
print("COMPILATION AND TEST VERIFICATION")
print("=" * 70)
print(f"Project directory: {proj_dir}")
print(f"Platform: {platform.system()}")
print(f"Python version: {sys.version}")

# List source files
print("\nSource files to compile:")
src_files = [
    "src\\ast.c", "src\\lexer.c", "src\\parser.c", 
    "src\\token.c", "src\\symbol.c",
    "tests\\test_parser.c"
]
for f in src_files:
    path = os.path.join(proj_dir, f)
    if os.path.exists(path):
        size = os.path.getsize(path)
        print(f"  ✓ {f} ({size} bytes)")
    else:
        print(f"  ✗ {f} NOT FOUND")

print("\n" + "=" * 70)
print("COMPILING...")
print("=" * 70)

cmd = [
    "gcc",
    "-Wall", "-Wextra", "-std=c99", "-g",
    "src\\ast.c", "src\\lexer.c", "src\\parser.c", 
    "src\\token.c", "src\\symbol.c",
    "tests\\test_parser.c",
    "-lm",
    "-o", "test_parser.exe"
]

print(f"\nCommand: {' '.join(cmd)}\n")

result = subprocess.run(cmd, capture_output=True, text=True)

print(result.stdout)
if result.stderr:
    print("STDERR Output:")
    print(result.stderr)

if result.returncode != 0:
    print(f"\n❌ Compilation FAILED (exit code: {result.returncode})")
    sys.exit(1)

print("\n✅ Compilation SUCCESSFUL")

# Check if executable was created
if os.path.exists("test_parser.exe"):
    size = os.path.getsize("test_parser.exe")
    print(f"✓ Executable created: test_parser.exe ({size} bytes)")
else:
    print("✗ Executable NOT created")
    sys.exit(1)

print("\n" + "=" * 70)
print("RUNNING TESTS...")
print("=" * 70 + "\n")

result = subprocess.run(["test_parser.exe"], capture_output=True, text=True)

print(result.stdout)
if result.stderr:
    print("STDERR:")
    print(result.stderr)

if result.returncode == 0:
    print("\n✅ ALL TESTS PASSED!")
else:
    print(f"\n❌ Tests FAILED (exit code: {result.returncode})")

sys.exit(result.returncode)
