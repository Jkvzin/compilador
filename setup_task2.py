#!/usr/bin/env python3
import os
import sys

# Create directories
os.makedirs("src", exist_ok=True)
os.makedirs("tests", exist_ok=True)

# Move test_lexer.c to tests directory
if os.path.exists("test_lexer.c"):
    with open("test_lexer.c", "r") as f:
        content = f.read()
    with open("tests/test_lexer.c", "w") as f:
        f.write(content)
    print("Moved test_lexer.c to tests/")

# Move token.h to src directory
if os.path.exists("token.h"):
    with open("token.h", "r") as f:
        content = f.read()
    with open("src/token.h", "w") as f:
        f.write(content)
    print("Moved token.h to src/")

print("Directories organized successfully!")
