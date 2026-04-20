#!/usr/bin/env python3
"""
Organize Task 1 files into proper directory structure
"""
import os
import shutil

os.chdir(os.path.dirname(os.path.abspath(__file__)))

# Create directories
os.makedirs('src', exist_ok=True)
os.makedirs('tests', exist_ok=True)
os.makedirs('.obj', exist_ok=True)
os.makedirs('examples', exist_ok=True)

# Move files to proper locations
moves = [
    ('token.h', 'src/token.h'),
    ('test_lexer.c', 'tests/test_lexer.c'),
]

for src, dst in moves:
    if os.path.exists(src):
        shutil.move(src, dst)
        print(f"✓ Moved {src} → {dst}")
    else:
        print(f"⚠ File not found: {src}")

print("\n✓ Project structure organized!")
print("\nFiles are now in:")
print("  - src/token.h")
print("  - tests/test_lexer.c")
print("  - Makefile (in root)")
print("\nNext: Run 'make test' to verify setup")
