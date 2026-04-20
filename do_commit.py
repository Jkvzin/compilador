#!/usr/bin/env python3
"""Commit AST implementation"""
import subprocess
import os

os.chdir(r"C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop")

# Stage files
print("Staging files...")
subprocess.run(["git", "add", "src/ast.h", "src/ast.c", "tests/test_ast.c"])

# Commit
print("Committing...")
msg = """feat: implement AST node structures

- Create 16 AST node types for all language constructs
- Implement factory functions for each node type
- Add utility functions (add_child, free, print)
- Create basic AST tests

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"""

result = subprocess.run(["git", "commit", "-m", msg])
print("Done!")
