@echo off
setlocal enabledelayedexpansion

cd /d "C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"

echo Compiling ast.c...
gcc -Wall -Wextra -std=c99 -g -fPIC -c src/ast.c -o .obj/ast.o
if errorlevel 1 (
    echo Compilation of ast.c failed!
    exit /b 1
)
echo ast.c compiled successfully

echo Compiling test_ast.c...
gcc -Wall -Wextra -std=c99 -g -fPIC -I src tests/test_ast.c .obj/ast.o -o test_ast.exe
if errorlevel 1 (
    echo Compilation of test_ast failed!
    exit /b 1
)
echo test_ast compiled successfully

echo.
echo Running tests...
test_ast.exe
if errorlevel 1 (
    echo Tests failed!
    exit /b 1
)
echo Tests passed!
exit /b 0
