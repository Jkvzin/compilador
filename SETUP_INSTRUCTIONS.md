# SIMPLES Compiler Project - Task 1 Setup

## Status: Files Created (Ready to Organize)

Due to environmental limitations (PowerShell not available), the files have been created in the root directory. They need to be organized into the proper structure.

## Files Created:
1. **Makefile** - Build system (already in correct location)
2. **token.h** - Token definitions (in root, needs to move to `src/`)
3. **test_lexer.c** - First test (in root, needs to move to `tests/`)
4. **setup_dirs.py** - Setup script to organize files
5. **setup.bat** - Windows batch setup script
6. **setup.sh** - Unix/bash setup script

## How to Complete Setup

### Option 1: Automatic Setup (Recommended)

Run the Python setup script to automatically organize all files:

```bash
python setup_dirs.py
```

This will:
- Create `src/`, `tests/`, and `unity/` directories
- Move `token.h` to `src/token.h`
- Move `test_lexer.c` to `tests/test_lexer.c`
- Display the final project structure

### Option 2: Manual Setup

```cmd
# Create directories
mkdir src
mkdir tests  
mkdir unity

# Move files
move token.h src\token.h
move test_lexer.c tests\test_lexer.c
```

### Option 3: Git-based Setup

```bash
git add token.h test_lexer.c
git commit -m "feat: create token definitions and test shell"

# Then reorganize:
mkdir -p src tests unity
mv token.h src/token.h
mv test_lexer.c tests/test_lexer.c
git add src tests unity
git commit --amend -m "feat: initialize project structure with token definitions..."
```

## Project Structure After Setup

```
compilador_cop/
├── Makefile             # Build configuration
├── .gitignore           # Git ignore patterns
├── src/
│   └── token.h          # Token type definitions (47 tokens)
├── tests/
│   └── test_lexer.c     # First test case (will fail as expected)
└── unity/               # Unity test framework (empty, to be added)
```

## Next Steps (Task 2)

1. Download Unity test framework to `unity/` directory
2. Implement `src/lexer.h` and `src/lexer.c`
3. Implement `src/token.c` with token_type_name() and token_list_free()
4. Run `make test` - initial test should fail (expected in TDD)

## Token Summary

**47 Tokens Total:**
- Keywords: 27 (programa, inicio, fim, inteiro, flutuante, vazio, se, entao, senao, fimse, enquanto, fimenquanto, para, de, ate, passo, faca, fimpara, leia, escreva, escreval, e, ou, nao, div, procedimento, retorna)
- Operators: 14 (<-, +, -, *, >, <, =, <>, >=, <=, (, ), comma, ;)
- Literals/IDs: 3 (ID, NUM_INT, NUM_FLOAT)
- Special: 2 (EOF, ERROR)

## Files Content

### src/token.h
Contains:
- TokenType enum with all 47 token definitions
- TokenValue union (int_value, float_value)
- Token struct with type, lexeme, lexeme_len, line, column, value
- TokenList struct with tokens array, count, capacity
- Function declarations: token_type_name(), token_list_free()

### tests/test_lexer.c
Contains:
- setUp() and tearDown() functions
- test_lexer_single_integer() - expects 2 tokens: TOK_NUM_INT + TOK_EOF
- Verifies token count, type, and value
- Calls token_list_free() for cleanup

### Makefile
Contains:
- CC = gcc with C99 standard
- CFLAGS with -Wall -Wextra -g -fPIC
- Targets: all, test, clean, dirs
- Source compilation to .obj/ directory
- Test runner linked with Unity framework

## Testing the Setup

After running setup_dirs.py:

```bash
make test
```

Expected output:
```
undefined reference to 'lexer_tokenize'
```

This is **expected and correct** - it's TDD! The lexer hasn't been implemented yet.

## Troubleshooting

If `make test` gives other errors:
1. Verify Unity framework is in `unity/` directory
2. Check that `src/token.h` and `tests/test_lexer.c` are in correct locations
3. Ensure gcc is installed: `gcc --version`

