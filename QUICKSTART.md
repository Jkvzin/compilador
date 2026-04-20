# QUICK START - SIMPLES Compiler Task 1

## Current Status
✓ Makefile created  
✓ token.h created (in root - needs moving to src/)  
✓ test_lexer.c created (in root - needs moving to tests/)  

## Complete Setup in 1 Step

```bash
python PROJECT_SETUP.py
```

That's it! This will:
1. Create src/, tests/, unity/ directories
2. Move token.h to src/token.h
3. Move test_lexer.c to tests/test_lexer.c
4. Display final structure

## Verify It Worked

```bash
make test
```

Should show:
```
undefined reference to 'lexer_tokenize'
```

This is **CORRECT** and **EXPECTED** (TDD - lexer not implemented yet!)

## What Was Created

### Makefile
- C99 compiler with -Wall -Wextra
- Builds to .obj/ directory
- Targets: all, test, clean

### src/token.h
- 47 token definitions (keywords, operators, literals, special)
- Token, TokenList, TokenValue structures
- Function declarations

### tests/test_lexer.c
- First TDD test case
- Tests: lexer_tokenize("42") → [NUM_INT(42), EOF]
- Uses Unity testing framework

## Files in Root (Temporary)
- token.h → will move to src/token.h
- test_lexer.c → will move to tests/test_lexer.c
- PROJECT_SETUP.py → run this to organize
- setup_dirs.py, setup.bat, setup.sh → alternative setup scripts

## Next: Task 2
Implement the lexer:
- src/lexer.h
- src/lexer.c  
- src/token.c

## Questions?
See TASK1_COMPLETION_REPORT.md for detailed documentation
See SETUP_INSTRUCTIONS.md for manual setup options
