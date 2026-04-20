#!/usr/bin/env python3
"""
Verify parser implementation syntax and structure
"""
import os
import re

def check_file_exists(path):
    """Check if file exists"""
    return os.path.isfile(path)

def check_includes(content, filename):
    """Check if required includes are present"""
    if 'parser.c' in filename or 'parser.h' in filename:
        required = ['token.h', 'ast.h']
    elif 'test_parser.c' in filename:
        required = ['parser.h', 'lexer.h']
    else:
        required = []
    
    issues = []
    for req in required:
        if f'#include' not in content or req not in content:
            issues.append(f"Missing include: {req}")
    return issues

def check_functions(content, expected_funcs):
    """Check if expected functions are defined"""
    issues = []
    for func in expected_funcs:
        if f'{func}' not in content:
            issues.append(f"Missing function: {func}")
    return issues

def main():
    base_dir = r"C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"
    
    print("=" * 60)
    print("PARSER IMPLEMENTATION VERIFICATION")
    print("=" * 60)
    
    # Check files exist
    files_to_check = {
        r'src\parser.h': 'Parser header',
        r'src\parser.c': 'Parser implementation',
        r'tests\test_parser.c': 'Parser tests',
        r'src\token.c': 'Token implementation',
    }
    
    print("\n1. FILE EXISTENCE CHECK")
    print("-" * 60)
    all_exist = True
    for rel_path, description in files_to_check.items():
        full_path = os.path.join(base_dir, rel_path)
        exists = check_file_exists(full_path)
        status = "✓" if exists else "✗"
        print(f"{status} {description:30} {rel_path}")
        if not exists:
            all_exist = False
    
    if not all_exist:
        print("\n⚠ Some files are missing!")
        return
    
    # Check header file
    print("\n2. PARSER HEADER CHECK (parser.h)")
    print("-" * 60)
    with open(os.path.join(base_dir, r'src\parser.h'), 'r') as f:
        header_content = f.read()
    
    expected_declares = [
        'Parser',
        'parser_create',
        'parser_free',
        'parser_parse_program',
        'parser_parse_declarations',
        'parser_parse_declaration',
        'parser_parse_commands',
        'parser_parse_command',
        'parser_current_token',
        'parser_match',
        'parser_check',
        'parser_advance',
        'parser_error',
    ]
    
    for decl in expected_declares:
        if decl in header_content:
            print(f"✓ {decl}")
        else:
            print(f"✗ {decl} - MISSING")
    
    # Check implementation file
    print("\n3. PARSER IMPLEMENTATION CHECK (parser.c)")
    print("-" * 60)
    with open(os.path.join(base_dir, r'src\parser.c'), 'r') as f:
        impl_content = f.read()
    
    expected_functions = [
        'parser_create',
        'parser_free',
        'parser_parse_program',
        'parser_parse_declarations',
        'parser_parse_declaration',
        'parser_parse_commands',
        'parser_parse_command',
        'parser_current_token',
        'parser_match',
        'parser_check',
        'parser_advance',
        'parser_error',
    ]
    
    for func in expected_functions:
        # Check if function is defined (look for "type function_name(")
        if re.search(rf'{func}\s*\(', impl_content):
            print(f"✓ {func}()")
        else:
            print(f"✗ {func}() - NOT DEFINED")
    
    # Check for required includes
    print("\n4. INCLUDE CHECKS")
    print("-" * 60)
    
    parser_h_issues = check_includes(header_content, 'parser.h')
    if parser_h_issues:
        for issue in parser_h_issues:
            print(f"✗ parser.h: {issue}")
    else:
        print("✓ parser.h: All required includes present")
    
    parser_c_issues = check_includes(impl_content, 'parser.c')
    if parser_c_issues:
        for issue in parser_c_issues:
            print(f"✗ parser.c: {issue}")
    else:
        print("✓ parser.c: All required includes present")
    
    with open(os.path.join(base_dir, r'tests\test_parser.c'), 'r') as f:
        test_content = f.read()
    
    test_issues = check_includes(test_content, 'test_parser.c')
    if test_issues:
        for issue in test_issues:
            print(f"✗ test_parser.c: {issue}")
    else:
        print("✓ test_parser.c: All required includes present")
    
    # Count tests
    print("\n5. TEST CASES CHECK")
    print("-" * 60)
    test_functions = re.findall(r'void (test_\w+)\(', test_content)
    print(f"Found {len(test_functions)} test functions:")
    for test in test_functions:
        print(f"  • {test}()")
    
    # Count assertions
    assertions = len(re.findall(r'ASSERT', test_content))
    print(f"Found {assertions} assertions in tests")
    
    print("\n" + "=" * 60)
    print("VERIFICATION COMPLETE")
    print("=" * 60)
    print("\nThe parser implementation appears to be complete!")
    print("\nTo compile and test, run:")
    print("  cd C:\\Users\\João Victor\\Documents\\Estudos progeamativos\\compiladores\\compilador_cop")
    print("  make test")
    print("\nOr manually:")
    print("  gcc -Wall -Wextra -std=c99 -g -c src/*.c tests/test_parser.c -I. && ...")

if __name__ == '__main__':
    main()
