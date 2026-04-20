@echo off
setlocal enabledelayedexpansion
cd /d "C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"
echo Compiling parser test...
gcc -Wall -Wextra -std=c99 -g -fPIC src\ast.c src\lexer.c src\parser.c src\token.c src\symbol.c tests\test_parser.c -lm -o test_parser.exe 2>&1
if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo Compilation successful!
    echo ========================================
    echo.
    echo Running tests...
    echo.
    test_parser.exe
) else (
    echo Compilation failed with error code %errorlevel%
)
