#!/usr/bin/env python3
import subprocess
import os
import sys

os.chdir(r"C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop")

# Check status
print("Current git status:")
result = subprocess.run(["git", "status", "--short"], capture_output=True, text=True)
print(result.stdout)

# Add files
print("\nAdding AST files...")
files_to_add = [
    "src/ast.h",
    "src/ast.c", 
    "tests/test_ast.c"
]

for f in files_to_add:
    result = subprocess.run(["git", "add", f], capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error adding {f}: {result.stderr}")
    else:
        print(f"✓ Added {f}")

# Check staged files
print("\nStaged files:")
result = subprocess.run(["git", "status", "--short"], capture_output=True, text=True)
print(result.stdout)

# Commit
print("\nCommitting...")
commit_msg = """feat: implement AST node structures

- Create 16 AST node types for all language constructs
- Implement factory functions for each node type
- Add utility functions (add_child, free, print)
- Create basic AST tests

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"""

result = subprocess.run(["git", "commit", "-m", commit_msg], capture_output=True, text=True)
if result.returncode != 0:
    print(f"Commit error: {result.stderr}")
    print(result.stdout)
else:
    print("✓ Commit successful")
    print(result.stdout)

# Show commit log
print("\nRecent commits:")
result = subprocess.run(["git", "log", "--oneline", "-n", "3"], capture_output=True, text=True)
print(result.stdout)
