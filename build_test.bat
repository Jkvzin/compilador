@echo off
cd C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop

REM Create object directory if it doesn't exist
if not exist .obj mkdir .obj

REM Compile all source files
echo Compiling source files...
gcc -Wall -Wextra -std=c99 -g -c src/token.c -o .obj/token.o
gcc -Wall -Wextra -std=c99 -g -c src/lexer.c -o .obj/lexer.o
gcc -Wall -Wextra -std=c99 -g -c src/ast.c -o .obj/ast.o
gcc -Wall -Wextra -std=c99 -g -c src/parser.c -o .obj/parser.o
gcc -Wall -Wextra -std=c99 -g -c src/symbol.c -o .obj/symbol.o

REM Compile test file
echo Compiling test file...
gcc -Wall -Wextra -std=c99 -g -c tests/test_parser.c -o .obj/test_parser.o

REM Link test executable
echo Linking test executable...
gcc -Wall -Wextra -std=c99 -g .obj/token.o .obj/lexer.o .obj/ast.o .obj/parser.o .obj/symbol.o .obj/test_parser.o -o test_parser -lm 2>&1

if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo Running tests...
    test_parser.exe
) else (
    echo Build failed!
)
