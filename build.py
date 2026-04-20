#!/usr/bin/env python3
"""
Build and test the SIMPLES compiler without Make.
Works on Windows, Linux, macOS.

Usage:
    python build.py                 # Build all
    python build.py test            # Run tests
    python build.py clean           # Clean build artifacts
    python build.py all             # Build all
"""

import os
import sys
import subprocess
import glob
import shutil

# Configuration
PROJECT_ROOT = os.path.dirname(os.path.abspath(__file__))
SRC_DIR = os.path.join(PROJECT_ROOT, 'src')
TESTS_DIR = os.path.join(PROJECT_ROOT, 'tests')
OBJ_DIR = os.path.join(PROJECT_ROOT, '.obj')

# Compiler settings
CC = 'gcc'
CFLAGS = ['-Wall', '-Wextra', '-std=c99', '-g', '-fPIC']
INCLUDES = ['-I' + SRC_DIR]

# Subprocess wrapper to enforce UTF-8 decoding and robust error handling
def run_cmd(cmd, **kwargs):
    """
    Run subprocess.run with capture_output/text and UTF-8 decoding.
    Uses errors='replace' to avoid UnicodeDecodeError on Windows consoles.
    """
    kwargs.setdefault('capture_output', True)
    kwargs.setdefault('text', True)
    kwargs.setdefault('encoding', 'utf-8')
    kwargs.setdefault('errors', 'replace')
    return subprocess.run(cmd, **kwargs)


def ensure_directories():
    """Create necessary directories."""
    os.makedirs(OBJ_DIR, exist_ok=True)
    os.makedirs(TESTS_DIR, exist_ok=True)
    print(f"✓ Directories ready")

def clean():
    """Remove build artifacts."""
    print("🧹 Cleaning...")
    if os.path.exists(OBJ_DIR):
        shutil.rmtree(OBJ_DIR)
        print(f"  Removed {OBJ_DIR}")
    
    # Remove executables
    for exe in ['test_lexer.exe', 'test_lexer', 'test_ast.exe', 'test_ast']:
        if os.path.exists(exe):
            os.remove(exe)
            print(f"  Removed {exe}")
    
    print("✓ Clean complete")

def compile_object(source):
    """Compile a single source file to object."""
    basename = os.path.basename(source)
    objname = os.path.splitext(basename)[0] + '.o'
    objfile = os.path.join(OBJ_DIR, objname)
    
    cmd = [CC] + CFLAGS + INCLUDES + ['-c', source, '-o', objfile]
    
    print(f"  Compiling {basename}...")
    result = run_cmd(cmd)
    
    if result.returncode != 0:
        print(f"❌ Compilation failed:")
        print(result.stderr)
        return None
    
    return objfile

def build():
    """Build all source files."""
    print("🔨 Building...")
    ensure_directories()
    
    # Find all .c files in src/
    sources = glob.glob(os.path.join(SRC_DIR, '*.c'))
    
    if not sources:
        print("❌ No source files found in src/")
        return False
    
    obj_files = []
    for source in sources:
        obj = compile_object(source)
        if obj is None:
            return False
        obj_files.append(obj)
    
    print("✓ Build complete")
    return True

def test():
    """Build and run tests."""
    print("🧪 Testing...")
    ensure_directories()
    
    all_passed = True
    
    # ===== Test Lexer =====
    print("\n📝 Running Lexer Tests...")
    
    # Compile lexer core
    lexer_src = os.path.join(SRC_DIR, 'lexer.c')
    lexer_obj = os.path.join(OBJ_DIR, 'lexer.o')
    
    if not os.path.exists(lexer_src):
        print(f"❌ {lexer_src} not found")
        print("   Run 'python FINAL_SETUP.py' first to organize files")
        return False
    
    # Compile lexer.c
    cmd = [CC] + CFLAGS + INCLUDES + ['-c', lexer_src, '-o', lexer_obj]
    result = run_cmd(cmd)
    if result.returncode != 0:
        print("❌ Lexer compilation failed:")
        print(result.stderr)
        return False
    print("  ✓ Compiled lexer.c")
    
    # Compile and link test
    test_file = os.path.join(TESTS_DIR, 'test_lexer.c')
    test_exe = os.path.join(PROJECT_ROOT, 'test_lexer')
    
    if not os.path.exists(test_file):
        print(f"❌ {test_file} not found")
        print("   Run 'python FINAL_SETUP.py' first to organize files")
        return False
    
    cmd = [CC] + CFLAGS + INCLUDES + [test_file, lexer_obj, '-o', test_exe]
    result = run_cmd(cmd)
    
    if result.returncode != 0:
        print("❌ Lexer test compilation failed:")
        print(result.stderr)
        return False
    print("  ✓ Compiled test_lexer.c")
    
    # Run lexer test
    print("\n" + "="*60)
    result = run_cmd([test_exe])
    print(result.stdout)
    if result.stderr:
        print(result.stderr)
    print("="*60)
    
    if result.returncode != 0:
        print("❌ Lexer tests failed")
        all_passed = False
    else:
        print("✅ Lexer tests passed!")
    
    # ===== Test AST =====
    print("\n🌳 Running AST Tests...")
    
    # Compile AST core
    ast_src = os.path.join(SRC_DIR, 'ast.c')
    ast_obj = os.path.join(OBJ_DIR, 'ast.o')
    
    if not os.path.exists(ast_src):
        print(f"⚠️  {ast_src} not found - skipping AST tests")
    else:
        # Compile ast.c
        cmd = [CC] + CFLAGS + INCLUDES + ['-c', ast_src, '-o', ast_obj]
        result = run_cmd(cmd)
        if result.returncode != 0:
            print("❌ AST compilation failed:")
            print(result.stderr)
            all_passed = False
        else:
            print("  ✓ Compiled ast.c")
            
            # Compile and link test
            ast_test_file = os.path.join(TESTS_DIR, 'test_ast.c')
            ast_test_exe = os.path.join(PROJECT_ROOT, 'test_ast')
            
            if not os.path.exists(ast_test_file):
                print(f"⚠️  {ast_test_file} not found - skipping AST tests")
            else:
                cmd = [CC] + CFLAGS + INCLUDES + [ast_test_file, ast_obj, '-o', ast_test_exe]
                result = run_cmd(cmd)
                
                if result.returncode != 0:
                    print("❌ AST test compilation failed:")
                    print(result.stderr)
                    all_passed = False
                else:
                    print("  ✓ Compiled test_ast.c")
                    
                    # Run AST test
                    print("\n" + "="*60)
                    result = run_cmd([ast_test_exe])
                    print(result.stdout)
                    if result.stderr:
                        print(result.stderr)
                    print("="*60)
                    
                    if result.returncode != 0:
                        print("❌ AST tests failed")
                        all_passed = False
                    else:
                        print("✅ AST tests passed!")
    
    if not all_passed:
        return False
    
    print("\n✅ All tests passed!")
    return True

def main():
    """Main entry point."""
    if len(sys.argv) < 2:
        target = 'all'
    else:
        target = sys.argv[1].lower()
    
    if target == 'clean':
        clean()
    elif target == 'test':
        success = test()
        sys.exit(0 if success else 1)
    elif target == 'all':
        success = build()
        sys.exit(0 if success else 1)
    elif target == 'help':
        print(__doc__)
    else:
        print(f"Unknown target: {target}")
        print(__doc__)
        sys.exit(1)

if __name__ == '__main__':
    main()
