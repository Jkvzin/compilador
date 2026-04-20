@echo off
cd /d "C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"
echo === COMPILATION OUTPUT ===
gcc -Wall -Wextra -std=c99 -g src\ast.c src\lexer.c src\parser.c src\token.c src\symbol.c tests\test_parser.c -lm -o test_parser.exe 2>&1
echo.
if %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
    echo.
    echo === TEST EXECUTION ===
    test_parser.exe
) else (
    echo Compilation failed with error code: %ERRORLEVEL%
)
pause
