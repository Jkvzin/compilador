@echo off
REM Task 8 - Quick Compilation Check

cd /d "C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"

if not exist .obj mkdir .obj

echo Checking for syntax errors...
echo.

setlocal enabledelayedexpansion

REM Compile each file separately to identify errors
set error_count=0

echo [1/5] Compiling token.c...
gcc -Wall -Wextra -std=c99 -g -c src/token.c -o .obj/token.o 2>token_errors.txt
if errorlevel 1 (
    echo   ERROR!
    type token_errors.txt
    set /a error_count=!error_count!+1
) else (
    echo   OK
)

echo [2/5] Compiling lexer.c...
gcc -Wall -Wextra -std=c99 -g -c src/lexer.c -o .obj/lexer.o 2>lexer_errors.txt
if errorlevel 1 (
    echo   ERROR!
    type lexer_errors.txt
    set /a error_count=!error_count!+1
) else (
    echo   OK
)

echo [3/5] Compiling ast.c...
gcc -Wall -Wextra -std=c99 -g -c src/ast.c -o .obj/ast.o 2>ast_errors.txt
if errorlevel 1 (
    echo   ERROR!
    type ast_errors.txt
    set /a error_count=!error_count!+1
) else (
    echo   OK
)

echo [4/5] Compiling parser.c...
gcc -Wall -Wextra -std=c99 -g -c src/parser.c -o .obj/parser.o 2>parser_errors.txt
if errorlevel 1 (
    echo   ERROR!
    type parser_errors.txt
    set /a error_count=!error_count!+1
) else (
    echo   OK
)

echo [5/5] Compiling symbol.c...
gcc -Wall -Wextra -std=c99 -g -c src/symbol.c -o .obj/symbol.o 2>symbol_errors.txt
if errorlevel 1 (
    echo   ERROR!
    type symbol_errors.txt
    set /a error_count=!error_count!+1
) else (
    echo   OK
)

echo.
if %error_count% equ 0 (
    echo ========================================
    echo  All source files compiled successfully!
    echo ========================================
) else (
    echo ========================================
    echo  %error_count% compilation error(s) found
    echo ========================================
    exit /b 1
)

REM Clean up error files
del /q token_errors.txt lexer_errors.txt ast_errors.txt parser_errors.txt symbol_errors.txt 2>nul

pause
