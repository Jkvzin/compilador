#!/usr/bin/env python3
"""
Compile and test AST implementation
"""

import subprocess
import os
import sys

PROJECT_ROOT = r"C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"
SRC_DIR = os.path.join(PROJECT_ROOT, 'src')
TESTS_DIR = os.path.join(PROJECT_ROOT, 'tests')
OBJ_DIR = os.path.join(PROJECT_ROOT, '.obj')

# Create object directory if it doesn't exist
os.makedirs(OBJ_DIR, exist_ok=True)

CC = 'gcc'
CFLAGS = ['-Wall', '-Wextra', '-std=c99', '-g', '-fPIC']
INCLUDES = ['-I' + SRC_DIR]

# Compile ast.c
print("Compiling ast.c...")
ast_src = os.path.join(SRC_DIR, 'ast.c')
ast_obj = os.path.join(OBJ_DIR, 'ast.o')
cmd = [CC] + CFLAGS + INCLUDES + ['-c', ast_src, '-o', ast_obj]
result = subprocess.run(cmd, capture_output=True, text=True)
if result.returncode != 0:
    print("❌ ast.c compilation failed:")
    print(result.stderr)
    sys.exit(1)
print("✓ ast.c compiled")

# Compile test_ast.c
print("Compiling test_ast.c...")
test_file = os.path.join(TESTS_DIR, 'test_ast.c')
test_exe = os.path.join(PROJECT_ROOT, 'test_ast')
cmd = [CC] + CFLAGS + INCLUDES + [test_file, ast_obj, '-o', test_exe]
result = subprocess.run(cmd, capture_output=True, text=True)
if result.returncode != 0:
    print("❌ test_ast.c compilation failed:")
    print(result.stderr)
    sys.exit(1)
print("✓ test_ast compiled")

# Run tests
print("\nRunning tests...\n" + "="*60)
result = subprocess.run([test_exe], capture_output=True, text=True)
print(result.stdout)
if result.stderr:
    print(result.stderr)
print("="*60)

if result.returncode != 0:
    print("\n❌ Tests failed")
    sys.exit(1)

print("\n✅ All tests passed!")
sys.exit(0)
