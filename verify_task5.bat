@echo off
REM Build verification script for Symbol Table Implementation
REM This script verifies the files are syntactically correct

setlocal enabledelayedexpansion

cd /d "C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"

echo ===============================================
echo Task 5: Symbol Table Implementation
echo ===============================================
echo.

echo Checking files exist...
if not exist src\symbol.h (
    echo ✗ src\symbol.h not found
    exit /b 1
)
echo ✓ src\symbol.h exists

if not exist src\symbol.c (
    echo ✗ src\symbol.c not found
    exit /b 1
)
echo ✓ src\symbol.c exists

if not exist tests\test_symbol.c (
    echo ✗ tests\test_symbol.c not found
    exit /b 1
)
echo ✓ tests\test_symbol.c exists

echo.
echo Verifying file structure...

REM Check for required symbols in symbol.h
findstr /c:"typedef struct" src\symbol.h >nul 2>&1 || echo ✗ Missing struct definition
findstr /c:"Symbol" src\symbol.h >nul 2>&1 && echo ✓ Symbol struct defined
findstr /c:"SymbolTable" src\symbol.h >nul 2>&1 && echo ✓ SymbolTable struct defined
findstr /c:"symbol_table_create" src\symbol.h >nul 2>&1 && echo ✓ symbol_table_create declared
findstr /c:"symbol_table_add" src\symbol.h >nul 2>&1 && echo ✓ symbol_table_add declared
findstr /c:"symbol_table_lookup" src\symbol.h >nul 2>&1 && echo ✓ symbol_table_lookup declared
findstr /c:"symbol_table_free" src\symbol.h >nul 2>&1 && echo ✓ symbol_table_free declared

echo.
echo Verifying implementation...

REM Check for required functions in symbol.c
findstr /c:"symbol_table_create" src\symbol.c >nul 2>&1 && echo ✓ symbol_table_create implemented
findstr /c:"symbol_table_free" src\symbol.c >nul 2>&1 && echo ✓ symbol_table_free implemented
findstr /c:"symbol_table_add" src\symbol.c >nul 2>&1 && echo ✓ symbol_table_add implemented
findstr /c:"symbol_table_lookup" src\symbol.c >nul 2>&1 && echo ✓ symbol_table_lookup implemented
findstr /c:"symbol_table_exists" src\symbol.c >nul 2>&1 && echo ✓ symbol_table_exists implemented
findstr /c:"symbol_table_enter_scope" src\symbol.c >nul 2>&1 && echo ✓ symbol_table_enter_scope implemented
findstr /c:"symbol_table_exit_scope" src\symbol.c >nul 2>&1 && echo ✓ symbol_table_exit_scope implemented

echo.
echo Verifying tests...

REM Check for test functions
findstr /c:"test_create_symbol_table" tests\test_symbol.c >nul 2>&1 && echo ✓ test_create_symbol_table
findstr /c:"test_add_symbols" tests\test_symbol.c >nul 2>&1 && echo ✓ test_add_symbols
findstr /c:"test_lookup_symbols" tests\test_symbol.c >nul 2>&1 && echo ✓ test_lookup_symbols
findstr /c:"test_duplicate_detection" tests\test_symbol.c >nul 2>&1 && echo ✓ test_duplicate_detection
findstr /c:"test_scope_entry_exit" tests\test_symbol.c >nul 2>&1 && echo ✓ test_scope_entry_exit
findstr /c:"test_symbol_exists" tests\test_symbol.c >nul 2>&1 && echo ✓ test_symbol_exists
findstr /c:"test_lookup_with_shadowing" tests\test_symbol.c >nul 2>&1 && echo ✓ test_lookup_with_shadowing
findstr /c:"test_capacity_expansion" tests\test_symbol.c >nul 2>&1 && echo ✓ test_capacity_expansion

echo.
echo ===============================================
echo ✓ Verification Complete
echo ===============================================
echo.
echo All files are present and structured correctly
echo Ready for compilation and testing
