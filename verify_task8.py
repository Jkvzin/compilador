#!/usr/bin/env python3
"""Verify Task 8 implementation - Statement Parsing"""

import os
import re

def check_file_exists(path):
    """Check if a file exists"""
    return os.path.exists(path)

def check_pattern_in_file(filepath, pattern, description):
    """Check if a pattern exists in a file"""
    if not check_file_exists(filepath):
        print(f"✗ File not found: {filepath}")
        return False
    
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
            if re.search(pattern, content, re.MULTILINE):
                print(f"✓ {description}")
                return True
            else:
                print(f"✗ {description}")
                return False
    except Exception as e:
        print(f"✗ Error reading {filepath}: {e}")
        return False

def main():
    base = r"C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"
    
    print("=" * 50)
    print("Task 8 - Statement Parsing Verification")
    print("=" * 50)
    
    all_checks = []
    
    # Check token.h
    print("\n[TOKEN.H]")
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "src/token.h"),
        r"TOK_ESCREVERLN",
        "TOK_ESCREVERLN token type defined"
    ))
    
    # Check lexer.c
    print("\n[LEXER.C]")
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "src/lexer.c"),
        r'"escreverln".*TOK_ESCREVERLN',
        "escreverln keyword mapping added"
    ))
    
    # Check parser.h
    print("\n[PARSER.H]")
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "src/parser.h"),
        r"parser_parse_read",
        "parser_parse_read declared"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "src/parser.h"),
        r"parser_parse_write",
        "parser_parse_write declared"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "src/parser.h"),
        r"parser_parse_if_statement",
        "parser_parse_if_statement declared"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "src/parser.h"),
        r"parser_parse_while_statement",
        "parser_parse_while_statement declared"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "src/parser.h"),
        r"parser_parse_for_statement",
        "parser_parse_for_statement declared"
    ))
    
    # Check parser.c
    print("\n[PARSER.C]")
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "src/parser.c"),
        r"ASTNode \*parser_parse_read\(",
        "parser_parse_read implemented"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "src/parser.c"),
        r"ASTNode \*parser_parse_write\(",
        "parser_parse_write implemented"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "src/parser.c"),
        r"ASTNode \*parser_parse_writeln\(",
        "parser_parse_writeln implemented"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "src/parser.c"),
        r"ASTNode \*parser_parse_if_statement\(",
        "parser_parse_if_statement implemented"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "src/parser.c"),
        r"ASTNode \*parser_parse_while_statement\(",
        "parser_parse_while_statement implemented"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "src/parser.c"),
        r"ASTNode \*parser_parse_for_statement\(",
        "parser_parse_for_statement implemented"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "src/parser.c"),
        r"parser_parse_read\(parser\)",
        "parser_parse_read called in parser_parse_command"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "src/parser.c"),
        r"parser_parse_write\(parser\)",
        "parser_parse_write called in parser_parse_command"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "src/parser.c"),
        r"parser_parse_if_statement\(parser\)",
        "parser_parse_if_statement called in parser_parse_command"
    ))
    
    # Check test_parser.c
    print("\n[TEST_PARSER.C]")
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "tests/test_parser.c"),
        r"void test_parse_read\(",
        "test_parse_read defined"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "tests/test_parser.c"),
        r"void test_parse_write\(",
        "test_parse_write defined"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "tests/test_parser.c"),
        r"void test_parse_if_statement\(",
        "test_parse_if_statement defined"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "tests/test_parser.c"),
        r"void test_parse_while_loop\(",
        "test_parse_while_loop defined"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "tests/test_parser.c"),
        r"void test_parse_for_loop\(",
        "test_parse_for_loop defined"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "tests/test_parser.c"),
        r"test_parse_read\(\)",
        "test_parse_read called in main"
    ))
    all_checks.append(check_pattern_in_file(
        os.path.join(base, "tests/test_parser.c"),
        r"test_parse_write\(\)",
        "test_parse_write called in main"
    ))
    
    print("\n" + "=" * 50)
    passed = sum(all_checks)
    total = len(all_checks)
    print(f"Verification: {passed}/{total} checks passed")
    print("=" * 50)
    
    if passed == total:
        print("✓ All checks passed!")
        return 0
    else:
        print(f"✗ {total - passed} checks failed")
        return 1

if __name__ == "__main__":
    exit(main())
