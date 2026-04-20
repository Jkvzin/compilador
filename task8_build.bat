@echo off
REM Simple compilation script for Task 8 - Statement Parsing

cd /d "C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"

REM Create object directory
if not exist .obj mkdir .obj

echo ==========================================
echo Compiling Task 8 - Statement Parsing
echo ==========================================

echo Compiling token.c...
gcc -Wall -Wextra -std=c99 -g -c src/token.c -o .obj/token.o
if errorlevel 1 (
    echo ERROR: Failed to compile token.c
    exit /b 1
)

echo Compiling lexer.c...
gcc -Wall -Wextra -std=c99 -g -c src/lexer.c -o .obj/lexer.o
if errorlevel 1 (
    echo ERROR: Failed to compile lexer.c
    exit /b 1
)

echo Compiling ast.c...
gcc -Wall -Wextra -std=c99 -g -c src/ast.c -o .obj/ast.o
if errorlevel 1 (
    echo ERROR: Failed to compile ast.c
    exit /b 1
)

echo Compiling parser.c...
gcc -Wall -Wextra -std=c99 -g -c src/parser.c -o .obj/parser.o
if errorlevel 1 (
    echo ERROR: Failed to compile parser.c
    exit /b 1
)

echo Compiling symbol.c...
gcc -Wall -Wextra -std=c99 -g -c src/symbol.c -o .obj/symbol.o
if errorlevel 1 (
    echo ERROR: Failed to compile symbol.c
    exit /b 1
)

echo Compiling test_parser.c...
gcc -Wall -Wextra -std=c99 -g -c tests/test_parser.c -o .obj/test_parser.o
if errorlevel 1 (
    echo ERROR: Failed to compile test_parser.c
    exit /b 1
)

echo Linking test executable...
gcc -Wall -Wextra -std=c99 -g .obj/token.o .obj/lexer.o .obj/ast.o .obj/parser.o .obj/symbol.o .obj/test_parser.o -o test_parser.exe -lm
if errorlevel 1 (
    echo ERROR: Failed to link test executable
    exit /b 1
)

echo ==========================================
echo Build successful!
echo ==========================================
echo.
echo Running tests...
echo ==========================================
test_parser.exe

pause
