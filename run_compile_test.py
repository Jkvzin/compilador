#!/usr/bin/env python3
import subprocess
import sys
import os

os.chdir(r"C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop")

print("=== COMPILATION OUTPUT ===")
compile_cmd = [
    "gcc",
    "-Wall", "-Wextra", "-std=c99", "-g",
    r"src\ast.c",
    r"src\lexer.c",
    r"src\parser.c",
    r"src\token.c",
    r"src\symbol.c",
    r"tests\test_parser.c",
    "-lm",
    "-o", "test_parser.exe"
]

result = subprocess.run(compile_cmd, capture_output=True, text=True)
print(result.stdout)
if result.stderr:
    print("STDERR:", result.stderr)

if result.returncode == 0:
    print("\nCompilation successful!")
    print("\n=== TEST EXECUTION ===")
    test_result = subprocess.run([r"test_parser.exe"], capture_output=True, text=True)
    print(test_result.stdout)
    if test_result.stderr:
        print("STDERR:", test_result.stderr)
    sys.exit(test_result.returncode)
else:
    print(f"\nCompilation failed with exit code: {result.returncode}")
    sys.exit(result.returncode)
