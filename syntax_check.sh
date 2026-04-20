#!/bin/bash
# Quick compilation test for Task 8

cd /c/Users/João\ Victor/Documents/Estudos\ progeamativos/compiladores/compilador_cop

echo "Checking for syntax errors..."

# Check token.h
gcc -Wall -Wextra -std=c99 -fsyntax-only src/token.h

# Check token.c
gcc -Wall -Wextra -std=c99 -I. -fsyntax-only src/token.c

# Check parser.h
gcc -Wall -Wextra -std=c99 -I. -fsyntax-only src/parser.h

# Check all source files
for file in src/*.c; do
    echo "Checking syntax of $file..."
    gcc -Wall -Wextra -std=c99 -c -fsyntax-only "$file" 2>&1 | head -5
done

echo "Done!"
