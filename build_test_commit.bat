@echo off
setlocal enabledelayedexpansion

REM Change to project directory
cd /d "C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"

echo.
echo ========================================
echo AST Implementation Build and Test
echo ========================================
echo.

REM Create object directory
if not exist .obj mkdir .obj

REM Compile AST
echo [1] Compiling src/ast.c...
gcc -Wall -Wextra -std=c99 -g -fPIC -I src -c src/ast.c -o .obj/ast.o
if errorlevel 1 (
    echo Error: Failed to compile ast.c
    exit /b 1
)
echo     OK

REM Compile test
echo [2] Compiling tests/test_ast.c...
gcc -Wall -Wextra -std=c99 -g -fPIC -I src tests/test_ast.c .obj/ast.o -o test_ast.exe
if errorlevel 1 (
    echo Error: Failed to compile test_ast.c
    exit /b 1
)
echo     OK

REM Run test
echo [3] Running tests...
echo ========================================
test_ast.exe
set TEST_RESULT=!errorlevel!
echo ========================================
echo.

if !TEST_RESULT! equ 0 (
    echo [4] Committing to git...
    git add src\ast.h src\ast.c tests\test_ast.c
    git commit -m "feat: implement AST node structures%NL%%NL%- Create 16 AST node types for all language constructs%NL%- Implement factory functions for each node type%NL%- Add utility functions (add_child, free, print)%NL%- Create basic AST tests%NL%%NL%Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
    echo [5] Build and commit complete!
    echo.
    git log --oneline -n 1
    exit /b 0
) else (
    echo ERROR: Tests failed with exit code !TEST_RESULT!
    exit /b 1
)
