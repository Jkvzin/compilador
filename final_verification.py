#!/usr/bin/env python3
"""
Final AST Implementation Verification
This script verifies that all AST files are correctly created and contain
the required implementations.
"""

import os
import sys

def check_file_exists(path):
    """Check if file exists and is readable"""
    if not os.path.exists(path):
        return False, "File does not exist"
    if not os.path.isfile(path):
        return False, "Not a file"
    size = os.path.getsize(path)
    return True, f"{size} bytes"

def check_file_contains(path, patterns):
    """Check if file contains all required patterns"""
    if not os.path.exists(path):
        return False, "File does not exist"
    
    with open(path, 'r') as f:
        content = f.read()
    
    missing = []
    for pattern in patterns:
        if pattern not in content:
            missing.append(pattern)
    
    if missing:
        return False, f"Missing: {', '.join(missing[:3])}"
    
    return True, "All patterns found"

def main():
    base = r"C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"
    
    print("=" * 70)
    print("AST IMPLEMENTATION VERIFICATION")
    print("=" * 70)
    
    # Check files exist
    print("\n[1] Checking files exist...")
    files_to_check = [
        ("src/ast.h", "AST Header"),
        ("src/ast.c", "AST Implementation"),
        ("tests/test_ast.c", "AST Tests"),
    ]
    
    all_exist = True
    for rel_path, desc in files_to_check:
        full_path = os.path.join(base, rel_path)
        exists, info = check_file_exists(full_path)
        status = "✓" if exists else "✗"
        print(f"  {status} {desc:25} {rel_path:20} {info}")
        all_exist = all_exist and exists
    
    if not all_exist:
        print("\n❌ Some files are missing!")
        return False
    
    # Check header file
    print("\n[2] Checking src/ast.h for required declarations...")
    header_patterns = [
        "typedef enum {",
        "TYPE_INT",
        "TYPE_FLOAT",
        "NODE_PROGRAM",
        "NODE_DECLARATION",
        "NODE_DECLARATIONS",
        "NODE_COMMANDS",
        "NODE_ASSIGNMENT",
        "NODE_READ",
        "NODE_WRITE",
        "NODE_WRITELN",
        "NODE_IF",
        "NODE_WHILE",
        "NODE_FOR",
        "NODE_BINOP",
        "NODE_UNOP",
        "NODE_ID",
        "NODE_NUM_INT",
        "NODE_NUM_FLOAT",
        "typedef struct ASTNode ASTNode",
        "ASTNode *ast_make_program",
        "ASTNode *ast_make_declaration",
        "ASTNode *ast_make_declarations",
        "ASTNode *ast_make_commands",
        "ASTNode *ast_make_assignment",
        "ASTNode *ast_make_read",
        "ASTNode *ast_make_write",
        "ASTNode *ast_make_if",
        "ASTNode *ast_make_while",
        "ASTNode *ast_make_for",
        "ASTNode *ast_make_binop",
        "ASTNode *ast_make_unop",
        "ASTNode *ast_make_id",
        "ASTNode *ast_make_num_int",
        "ASTNode *ast_make_num_float",
        "void ast_add_child",
        "void ast_free",
        "void ast_print",
    ]
    
    header_path = os.path.join(base, "src/ast.h")
    header_ok, header_msg = check_file_contains(header_path, header_patterns)
    print(f"  {'✓' if header_ok else '✗'} All declarations present: {header_msg}")
    
    # Check implementation file
    print("\n[3] Checking src/ast.c for required implementations...")
    impl_patterns = [
        "#include \"ast.h\"",
        "#include <stdlib.h>",
        "#include <string.h>",
        "#include <stdio.h>",
        "static char *str_dup",
        "static ASTNode *ast_alloc",
        "ASTNode *ast_make_program(",
        "ASTNode *ast_make_declaration(",
        "ASTNode *ast_make_declarations(",
        "ASTNode *ast_make_commands(",
        "ASTNode *ast_make_assignment(",
        "ASTNode *ast_make_read(",
        "ASTNode *ast_make_write(",
        "ASTNode *ast_make_if(",
        "ASTNode *ast_make_while(",
        "ASTNode *ast_make_for(",
        "ASTNode *ast_make_binop(",
        "ASTNode *ast_make_unop(",
        "ASTNode *ast_make_id(",
        "ASTNode *ast_make_num_int(",
        "ASTNode *ast_make_num_float(",
        "void ast_add_child(",
        "void ast_print(",
        "void ast_free(",
        "realloc",
    ]
    
    impl_path = os.path.join(base, "src/ast.c")
    impl_ok, impl_msg = check_file_contains(impl_path, impl_patterns)
    print(f"  {'✓' if impl_ok else '✗'} All implementations present: {impl_msg}")
    
    # Check test file
    print("\n[4] Checking tests/test_ast.c for test functions...")
    test_patterns = [
        "#include \"../src/ast.h\"",
        "void test_create_simple_program",
        "void test_add_declarations",
        "void test_add_commands",
        "void test_expressions",
        "void test_control_flow",
        "void test_loops",
        "void test_numeric_types",
        "int main(void)",
    ]
    
    test_path = os.path.join(base, "tests/test_ast.c")
    test_ok, test_msg = check_file_contains(test_path, test_patterns)
    print(f"  {'✓' if test_ok else '✗'} All test functions present: {test_msg}")
    
    # Summary
    print("\n" + "=" * 70)
    all_ok = header_ok and impl_ok and test_ok
    
    if all_ok:
        print("✅ ALL VERIFICATIONS PASSED!")
        print("\nFiles ready for:")
        print("  1. Compilation with gcc")
        print("  2. Git commit")
        print("  3. Integration with parser")
        return True
    else:
        print("❌ SOME VERIFICATIONS FAILED!")
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
