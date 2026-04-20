@echo off
REM Task 2 Setup Script
REM Creates directories and lexer implementation files

echo Creating directories...
if not exist src mkdir src
if not exist tests mkdir tests
if not exist .obj mkdir .obj

echo.
echo Copying files to proper locations...

if exist token.h (
    if not exist src\token.h (
        copy token.h src\token.h >nul
        echo Created src\token.h
    )
)

if exist test_lexer.c (
    if not exist tests\test_lexer.c (
        copy test_lexer.c tests\test_lexer.c >nul
        echo Created tests\test_lexer.c
    )
)

echo.
echo Creating src\lexer.h...

(
echo #ifndef LEXER_H
echo #define LEXER_H
echo.
echo #include "token.h"
echo.
echo typedef struct {
echo     const char *input;
echo     size_t current;
echo     size_t start;
echo     int line;
echo     int column;
echo } Lexer;
echo.
echo Lexer *lexer_create^(const char *input^);
echo void lexer_free^(Lexer *lexer^);
echo TokenList *lexer_tokenize^(const char *input^);
echo.
echo #endif
) > src\lexer.h

echo.
echo Setup complete!
echo Next: Run 'make clean test' to build and run tests
