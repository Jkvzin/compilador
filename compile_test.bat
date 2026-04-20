@echo off
cd /d "C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"
gcc -Wall -Wextra -std=c99 -g -fPIC src\*.c tests\test_parser.c -lm -o test_parser.exe
echo Compilation result: %errorlevel%
if %errorlevel% equ 0 (
    echo Compilation successful, running tests...
    test_parser.exe
) else (
    echo Compilation failed
)
