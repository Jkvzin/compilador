#!/usr/bin/env python
import subprocess
import os

os.chdir(r"C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop")

try:
    result = subprocess.call([
        "gcc",
        "-Wall", "-Wextra", "-std=c99", "-g", "-fPIC",
        "src\\ast.c", "src\\lexer.c", "src\\parser.c", "src\\token.c", "src\\symbol.c",
        "tests\\test_parser.c",
        "-lm",
        "-o", "test_parser.exe"
    ])
    
    if result == 0:
        print("\n\nCompilation successful! Running tests...\n\n")
        subprocess.call(["test_parser.exe"])
    else:
        print("Compilation failed!")
        
except Exception as e:
    print("Error:", e)
