#!/usr/bin/env python3
"""
Quick verification that all AST files exist and are valid
"""
import os

base = r"C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"

print("=" * 70)
print("QUICK AST VERIFICATION")
print("=" * 70)

files = {
    "src/ast.h": 150,
    "src/ast.c": 400,
    "tests/test_ast.c": 200,
}

print("\n1. File Existence Check")
all_ok = True
for f, min_lines in files.items():
    path = os.path.join(base, f)
    if os.path.exists(path):
        size = os.path.getsize(path)
        lines = sum(1 for _ in open(path))
        ok = lines >= min_lines
        status = "✓" if ok else "⚠"
        print(f"  {status} {f:25} {size:7} bytes, {lines:3} lines")
        if not ok:
            print(f"     Warning: Expected at least {min_lines} lines")
            all_ok = False
    else:
        print(f"  ✗ {f:25} NOT FOUND")
        all_ok = False

print("\n2. Key Content Checks")

# Check header
with open(os.path.join(base, "src/ast.h")) as f:
    header = f.read()
    
checks = [
    ("NODE_PROGRAM" in header, "NODE_PROGRAM defined"),
    ("NODE_NUM_FLOAT" in header, "NODE_NUM_FLOAT defined"),
    ("ast_make_program" in header, "ast_make_program declared"),
    ("ast_free" in header, "ast_free declared"),
    ("ast_print" in header, "ast_print declared"),
]

for ok, desc in checks:
    print(f"  {'✓' if ok else '✗'} {desc}")
    all_ok = all_ok and ok

# Check implementation
with open(os.path.join(base, "src/ast.c")) as f:
    impl = f.read()
    
impl_checks = [
    ("static char *str_dup" in impl, "str_dup helper exists"),
    ("void ast_free(" in impl, "ast_free implemented"),
    ("ASTNode *ast_make_program(" in impl, "ast_make_program implemented"),
    ("void ast_add_child(" in impl, "ast_add_child implemented"),
    ("realloc" in impl, "Dynamic memory management"),
]

for ok, desc in impl_checks:
    print(f"  {'✓' if ok else '✗'} {desc}")
    all_ok = all_ok and ok

print("\n3. Syntax Validation")

# Count braces to detect syntax errors
open_braces = header.count('{') + impl.count('{')
close_braces = header.count('}') + impl.count('}')
brace_balanced = open_braces == close_braces
print(f"  {'✓' if brace_balanced else '✗'} Braces balanced ({open_braces} open, {close_braces} close)")
all_ok = all_ok and brace_balanced

print("\n" + "=" * 70)
if all_ok:
    print("✅ ALL VERIFICATIONS PASSED")
    print("\nReady to build! Run one of:")
    print("  python build_and_commit.py")
    print("  python build.py test")
    print("  Or manually compile with gcc")
else:
    print("❌ SOME CHECKS FAILED")
    print("Please review the warnings above")

print("=" * 70)
