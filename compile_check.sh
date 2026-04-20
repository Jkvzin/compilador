#!/bin/bash
cd "C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"

# Try to compile symbol files
gcc -Wall -Wextra -std=c99 -c src/symbol.c -o .obj/symbol.o -I.

if [ $? -eq 0 ]; then
    echo "✓ symbol.c compiled successfully"
else
    echo "✗ symbol.c compilation failed"
    exit 1
fi

# Try to compile test file
gcc -Wall -Wextra -std=c99 -c tests/test_symbol.c -o .obj/test_symbol.o -I. -Isrc

if [ $? -eq 0 ]; then
    echo "✓ test_symbol.c compiled successfully"
else
    echo "✗ test_symbol.c compilation failed"
    exit 1
fi

echo "✓ All compilation checks passed"
