@echo off
REM Create directories
if not exist src mkdir src
if not exist tests mkdir tests

REM Move existing files if they exist
if exist token.h (
    move /Y token.h src\token.h
    echo Moved token.h to src\
)

if exist test_lexer.c (
    move /Y test_lexer.c tests\test_lexer.c
    echo Moved test_lexer.c to tests\
)

echo Directories created and files organized
