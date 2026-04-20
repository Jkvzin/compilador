#!/usr/bin/env python3
"""
Complete AST verification and testing script
"""
import subprocess
import os
import sys

def run_cmd(cmd, description=""):
    """Run a command and return result"""
    if description:
        print(f"\n{description}")
    result = subprocess.run(cmd, capture_output=True, text=True, cwd=os.getcwd())
    return result

def main():
    os.chdir(r"C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop")
    
    print("="*70)
    print("AST Implementation Verification")
    print("="*70)
    
    # 1. Verify files exist
    print("\n[1] Verifying files exist...")
    files = [
        "src/ast.h",
        "src/ast.c",
        "tests/test_ast.c"
    ]
    
    for f in files:
        exists = os.path.exists(f)
        status = "✓" if exists else "✗"
        size = os.path.getsize(f) if exists else 0
        print(f"  {status} {f} ({size} bytes)")
    
    # 2. Check for required components in header
    print("\n[2] Checking ast.h for required components...")
    with open("src/ast.h", "r") as f:
        header = f.read()
    
    checks = [
        ("DataType enum", "typedef enum {" in header and "TYPE_INT" in header),
        ("ASTNodeType enum", "NODE_PROGRAM" in header and "NODE_NUM_FLOAT" in header),
        ("ASTNode struct", "typedef struct ASTNode" in header),
        ("ast_make_program", "ast_make_program" in header),
        ("ast_make_declarations", "ast_make_declarations" in header),
        ("ast_add_child", "ast_add_child" in header),
        ("ast_free", "ast_free" in header),
        ("ast_print", "ast_print" in header),
    ]
    
    for check_name, result in checks:
        status = "✓" if result else "✗"
        print(f"  {status} {check_name}")
    
    # 3. Check for required components in implementation
    print("\n[3] Checking ast.c for required implementations...")
    with open("src/ast.c", "r") as f:
        impl = f.read()
    
    impl_checks = [
        ("str_dup helper", "str_dup" in impl),
        ("ast_alloc helper", "ast_alloc" in impl),
        ("ast_make_program impl", "ASTNode *ast_make_program(" in impl),
        ("ast_make_binop impl", "ASTNode *ast_make_binop(" in impl),
        ("ast_add_child impl", "void ast_add_child(" in impl),
        ("ast_free impl", "void ast_free(" in impl),
        ("ast_print impl", "void ast_print(" in impl),
    ]
    
    for check_name, result in impl_checks:
        status = "✓" if result else "✗"
        print(f"  {status} {check_name}")
    
    # 4. Attempt compilation
    print("\n[4] Attempting compilation...")
    os.makedirs(".obj", exist_ok=True)
    
    # Compile ast.c
    print("  Compiling ast.c...")
    cmd = ["gcc", "-Wall", "-Wextra", "-std=c99", "-g", "-fPIC", 
           "-I", "src", "-c", "src/ast.c", "-o", ".obj/ast.o"]
    result = run_cmd(cmd)
    if result.returncode == 0:
        print("    ✓ ast.c compiled successfully")
    else:
        print("    ✗ ast.c compilation failed")
        print("    Error:", result.stderr[:200] if result.stderr else "Unknown")
        return False
    
    # Compile test
    print("  Compiling test_ast.c...")
    cmd = ["gcc", "-Wall", "-Wextra", "-std=c99", "-g", "-fPIC",
           "-I", "src", "tests/test_ast.c", ".obj/ast.o", "-o", "test_ast"]
    result = run_cmd(cmd)
    if result.returncode == 0:
        print("    ✓ test_ast compiled successfully")
    else:
        print("    ✗ test_ast compilation failed")
        print("    Error:", result.stderr[:200] if result.stderr else "Unknown")
        return False
    
    # 5. Run tests
    print("\n[5] Running tests...")
    print("="*70)
    result = run_cmd(["test_ast"])
    print(result.stdout)
    if result.stderr:
        print("Stderr:", result.stderr)
    print("="*70)
    
    if result.returncode == 0:
        print("\n✅ All tests passed!")
        return True
    else:
        print("\n❌ Tests failed!")
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
