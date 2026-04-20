#!/usr/bin/env python3
"""
Complete AST Build, Test, and Commit Script
"""

import subprocess
import os
import sys

def run_cmd(cmd, desc="", show_output=True):
    """Run a command and return result"""
    if desc:
        print(desc, end=" ... ")
    result = subprocess.run(cmd, capture_output=True, text=True)
    if show_output:
        if result.returncode == 0:
            print("✓")
        else:
            print("✗")
            if result.stderr:
                print(f"  Error: {result.stderr[:200]}")
    return result

def main():
    os.chdir(r"C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop")
    
    print("=" * 70)
    print("AST IMPLEMENTATION - BUILD, TEST, AND COMMIT")
    print("=" * 70)
    print()
    
    # Create build directory
    os.makedirs(".obj", exist_ok=True)
    
    # Compile AST
    print("[1] Compiling AST Implementation")
    result = run_cmd(
        ["gcc", "-Wall", "-Wextra", "-std=c99", "-g", "-fPIC", 
         "-I", "src", "-c", "src/ast.c", "-o", ".obj/ast.o"],
        "  • Compiling src/ast.c"
    )
    if result.returncode != 0:
        print("Compilation failed!")
        return False
    
    # Compile test
    print("\n[2] Compiling AST Tests")
    result = run_cmd(
        ["gcc", "-Wall", "-Wextra", "-std=c99", "-g", "-fPIC",
         "-I", "src", "tests/test_ast.c", ".obj/ast.o", "-o", "test_ast"],
        "  • Compiling tests/test_ast.c"
    )
    if result.returncode != 0:
        print("Test compilation failed!")
        return False
    
    # Run tests
    print("\n[3] Running AST Tests")
    print("-" * 70)
    result = run_cmd(["test_ast"], show_output=False)
    print(result.stdout)
    print("-" * 70)
    
    if result.returncode != 0:
        print("Tests failed!")
        return False
    
    # Git operations
    print("\n[4] Git Operations")
    
    # Stage files
    result = run_cmd(
        ["git", "add", "src/ast.h", "src/ast.c", "tests/test_ast.c"],
        "  • Staging files"
    )
    if result.returncode != 0:
        print("Failed to stage files!")
        return False
    
    # Commit
    msg = """feat: implement AST node structures

- Create 16 AST node types for all language constructs
- Implement factory functions for each node type
- Add utility functions (add_child, free, print)
- Create basic AST tests

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"""
    
    result = run_cmd(
        ["git", "commit", "-m", msg],
        "  • Creating git commit"
    )
    if result.returncode != 0:
        print("Failed to commit!")
        print(result.stderr)
        return False
    
    # Show commit
    print("\n[5] Commit Details")
    result = subprocess.run(["git", "log", "--oneline", "-n", "1"], 
                          capture_output=True, text=True)
    print("  " + result.stdout.strip())
    
    print("\n" + "=" * 70)
    print("✅ SUCCESS: AST Implementation Complete!")
    print("=" * 70)
    print()
    print("Summary:")
    print("  ✓ ast.h: 16 node types and all factory functions")
    print("  ✓ ast.c: Complete implementation with memory management")
    print("  ✓ test_ast.c: Comprehensive test suite")
    print("  ✓ All tests passing")
    print("  ✓ Changes committed to git")
    print()
    
    return True

if __name__ == "__main__":
    try:
        success = main()
        sys.exit(0 if success else 1)
    except Exception as e:
        print(f"\n❌ ERROR: {e}")
        sys.exit(1)
