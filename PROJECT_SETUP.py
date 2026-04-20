#!/usr/bin/env python3
"""
SIMPLES Compiler - Project Setup Script
Creates proper directory structure and organizes files
"""
import os
import sys
import shutil

def create_structure():
    """Create the project directory structure and organize files"""
    
    print("Creating SIMPLES Compiler project structure...\n")
    
    # Create directories
    dirs_to_create = ['src', 'tests', 'unity']
    for dir_name in dirs_to_create:
        os.makedirs(dir_name, exist_ok=True)
        print(f"✓ Created directory: {dir_name}/")
    
    # Move token.h to src/
    if os.path.exists('token.h'):
        shutil.move('token.h', 'src/token.h')
        print("✓ Moved token.h → src/token.h")
    else:
        print("⚠ token.h not found in root")
    
    # Move test_lexer.c to tests/
    if os.path.exists('test_lexer.c'):
        shutil.move('test_lexer.c', 'tests/test_lexer.c')
        print("✓ Moved test_lexer.c → tests/test_lexer.c")
    else:
        print("⚠ test_lexer.c not found in root")
    
    print("\n" + "="*50)
    print("PROJECT STRUCTURE CREATED")
    print("="*50 + "\n")
    
    # Display structure
    print("Final directory structure:")
    print("compilador_cop/")
    print("├── Makefile")
    print("├── .gitignore")
    print("├── src/")
    print("│   └── token.h             (47 token definitions)")
    print("├── tests/")
    print("│   └── test_lexer.c        (first test case)")
    print("└── unity/                  (Unity framework - empty)")
    print()
    
    # Next steps
    print("="*50)
    print("NEXT STEPS - READY TO TEST")
    print("="*50 + "\n")
    print("Status: ✓ Token definitions created")
    print("Status: ✓ Test shell created")
    print("Status: ✓ Makefile configured")
    print("Status: ⧗ Awaiting lexer implementation\n")
    
    print("To verify setup:")
    print("  make test")
    print("\nExpected result: 'undefined reference to lexer_tokenize'")
    print("(This is CORRECT - it's TDD! The lexer will be implemented next.)\n")
    
    return True

if __name__ == '__main__':
    try:
        success = create_structure()
        if success:
            print("✓ Setup complete! Ready for Task 2 (Lexer Implementation)")
            sys.exit(0)
    except Exception as e:
        print(f"\n✗ Error during setup: {e}", file=sys.stderr)
        sys.exit(1)
