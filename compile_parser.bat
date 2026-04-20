@echo off
REM Compile parser test

cd /d "C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"

REM Create object directory
if not exist .obj mkdir .obj

echo Compiling sources...
gcc -Wall -Wextra -std=c99 -g -c src/token.c -o .obj/token.o
if errorlevel 1 exit /b 1

gcc -Wall -Wextra -std=c99 -g -c src/lexer.c -o .obj/lexer.o
if errorlevel 1 exit /b 1

gcc -Wall -Wextra -std=c99 -g -c src/ast.c -o .obj/ast.o
if errorlevel 1 exit /b 1

gcc -Wall -Wextra -std=c99 -g -c src/symbol.c -o .obj/symbol.o
if errorlevel 1 exit /b 1

gcc -Wall -Wextra -std=c99 -g -c src/parser.c -o .obj/parser.o
if errorlevel 1 exit /b 1

echo Compiling tests...
gcc -Wall -Wextra -std=c99 -g -c tests/test_parser.c -o .obj/test_parser.o
if errorlevel 1 exit /b 1

echo Linking test executable...
gcc -Wall -Wextra -std=c99 -g .obj/token.o .obj/lexer.o .obj/ast.o .obj/symbol.o .obj/parser.o .obj/test_parser.o -o test_parser.exe -lm
if errorlevel 1 exit /b 1

echo Running tests...
test_parser.exe

echo Done!
pause
