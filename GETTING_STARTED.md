# 🚀 Getting Started - SIMPLES Compiler

## Quick Start (5 minutes)

### Step 1: Organize Project
```bash
cd C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop
python FINAL_SETUP.py
```

**What it does:** Moves token.h and test_lexer.c into proper src/ and tests/ directories.

### Step 2: Verify Build
```bash
make clean test
```

**Expected output:** Lexer tests should pass (or show what needs to be completed).

### Step 3: Check Status
```bash
cat README.md
cat LEXER_COMPLETE.md
```

---

## What's Done ✅

- ✅ **Token Definitions:** All 47 tokens in `src/token.h`
- ✅ **Lexer Implementation:** Complete scanner in `src/lexer.c`
- ✅ **Build System:** Working Makefile with test target
- ✅ **Project Structure:** Proper src/, tests/, examples/ directories
- ✅ **Documentation:** Comprehensive guides and specifications

---

## What's Next ⏳

### Phase 1: Lexer (Finishing)
- [ ] **Task 3:** Expand lexer test suite
  - Add operator tests
  - Add keyword tests
  - Add full program tests
  - Estimated: 20 minutes

### Phase 2: Parser (Next - 2-3 hours)
- [ ] **Task 4:** AST node structures
- [ ] **Task 5:** Symbol table
- [ ] **Task 6-9:** Recursive descent parser
- [ ] **Task 10:** Parser tests

### Phase 3: Semantic Analysis (1 hour)
- [ ] **Task 11-12:** Type checking and validation

### Phase 4: Code Generation (3-4 hours)
- [ ] **Task 13-20:** NASM assembly generation

---

## File Organization

```
compilador_cop/
├── src/                   # Source code
│   ├── token.h           # ✅ Token definitions
│   ├── lexer.h           # ✅ Lexer interface
│   ├── lexer.c           # ✅ Lexer implementation
│   ├── parser.h/c        # ⏳ Parser (next)
│   ├── ast.h/c           # ⏳ AST nodes (next)
│   ├── semantic.h/c      # ⏳ Semantic analyzer
│   ├── codegen.h/c       # ⏳ Code generator
│   └── main.c            # ⏳ CLI entry point
│
├── tests/                # Test files
│   ├── test_lexer.c      # ✅ Lexer tests
│   ├── test_parser.c     # ⏳ Parser tests
│   ├── test_semantic.c   # ⏳ Semantic tests
│   └── test_codegen.c    # ⏳ Codegen tests
│
├── examples/             # Example programs
│   ├── assignment.simples
│   ├── conditional.simples
│   ├── loop_while.simples
│   ├── loop_for.simples
│   ├── fatorial.simples
│   └── fibonacci.simples
│
├── Makefile              # ✅ Build system
├── .gitignore            # ✅ Git configuration
│
└── Documentation/
    ├── README.md         # ✅ Main documentation
    ├── GETTING_STARTED.md # ✅ This file
    ├── DOCUMENTATION.md  # ✅ Full specification
    ├── plan.md           # ✅ 20-task breakdown
    ├── STATUS.md         # ✅ Progress tracking
    ├── LEXER_COMPLETE.md # ✅ Lexer summary
    └── SESSION_COMPLETE.md # ✅ Session report
```

---

## Understanding the Codebase

### 1. Tokens (`src/token.h`)
- 27 keywords (programa, inteiro, se, etc.)
- 14 operators (<-, +, -, *, >, <, etc.)
- 3 literals (ID, NUM_INT, NUM_FLOAT)
- 2 special tokens (EOF, ERROR)

### 2. Lexer (`src/lexer.c`)
- Scans source code character-by-character
- Recognizes keywords and operators
- Tracks line/column for error reporting
- Returns TokenList with all tokens

### 3. Architecture
```
Source Code
    ↓
Lexer (tokenize) → Token Stream
    ↓
Parser (parse) → AST
    ↓
Semantic Analyzer (check types) → Validated AST
    ↓
Code Generator (emit) → NASM Assembly
    ↓
NASM Assembler → Executable
```

---

## Language Features

SIMPLES supports:
- **Variables:** `inteiro x`, `flutuante y`
- **Assignment:** `x <- 42`
- **Arithmetic:** `+`, `-`, `*`, `div`
- **Comparison:** `>`, `<`, `=`, `<>`, `>=`, `<=`
- **Logic:** `e`, `ou`, `nao`
- **Conditionals:** `se ... entao ... senao ... fimse`
- **Loops:** `enquanto`, `para ... ate ... passo`
- **I/O:** `leia`, `escreva`, `escreval`

### Example Program
```simples
programa fatorial
  inteiro n, fat, i
inicio
  leia n
  fat <- 1
  para i de 2 ate n passo 1 faca
    fat <- fat * i
  fimpara
  escreva fat
fim
```

---

## Build Commands

```bash
# Compile everything
make all

# Run tests
make test

# Remove build artifacts
make clean

# Show help
make help
```

---

## Implementation Checklist

### Before Starting Parser (Tasks 4-10)

- [ ] Run `python FINAL_SETUP.py`
- [ ] Run `make clean test`
- [ ] Verify lexer tests pass
- [ ] Review `DOCUMENTATION.md`
- [ ] Read `plan.md` thoroughly
- [ ] Understand AST structure in `plan.md` (Task 4)
- [ ] Understand parser logic in `plan.md` (Tasks 6-9)

### Each Task

1. **Read the plan:** See `plan.md` for exact code
2. **Write tests first:** Add test to appropriate test_*.c
3. **Run tests:** `make test` (expect fail)
4. **Implement:** Write minimal code to pass
5. **Commit:** `git add src/ tests/` + `git commit`

---

## Recommended Reading Order

1. **README.md** (this folder) - Overview
2. **GETTING_STARTED.md** (this file) - Setup
3. **DOCUMENTATION.md** - Language spec
4. **plan.md** - Implementation details
5. **Code:** src/token.h → src/lexer.c → tests/test_lexer.c

---

## Common Issues

### "make: command not found"
- Install GNU Make (comes with most development tools)
- On Windows: Install via MinGW or use WSL

### "gcc: command not found"
- Install GCC compiler
- On Windows: Use MinGW, WSL, or Visual Studio Build Tools

### Tests don't compile
- Run `python FINAL_SETUP.py` first to organize files
- Ensure token.h is in src/ and test_lexer.c is in tests/

### Makefile issues
- Ensure proper indentation (tabs, not spaces)
- Use forward slashes in paths (Makefile is cross-platform)

---

## Next Session Plan

When you return to continue:

1. **First:** Run `python FINAL_SETUP.py` to ensure files are organized
2. **Then:** `make test` to verify current state
3. **Review:** Read updated plan.md for next task
4. **Code:** Complete the next pending task from the list

Current pending tasks (in order):
1. Task 3: Complete Lexer Test Suite (20 min)
2. Task 4: AST Node Structures (30 min)
3. Task 5: Symbol Table (20 min)
4. Tasks 6-10: Parser Implementation (2-3 hours)
5. Tasks 11-20: Semantic Analysis & Code Generation (4-5 hours)

---

## Success Criteria

✅ All tests passing
✅ Code compiles without warnings
✅ Files organized in proper directories
✅ Git history clean and meaningful
✅ Documentation up-to-date

---

## Resources

- **DOCUMENTATION.md** - Full language specification
- **plan.md** - Detailed task descriptions with code examples
- **STATUS.md** - Current progress tracker
- **PRD/prd.md** - Original requirements

---

## Getting Help

**If stuck on a task:**
1. Check `plan.md` for that task
2. Review similar completed tasks
3. Read DOCUMENTATION.md for language details
4. Examine existing test files for patterns

**If tests fail:**
1. Verify files are in correct directories
2. Check for syntax errors
3. Run `make clean` then `make test` again
4. Compare against `plan.md` code examples

---

## Summary

✅ **Done:** Lexer phase (66% complete)
⏳ **Next:** Parser phase (Tasks 4-10)
📚 **Documents:** 13 comprehensive guides
🔨 **Build:** Ready to use
🧪 **Tests:** TDD framework in place

**You're ready to start implementation!**

---

**Quick Start:**
```bash
python FINAL_SETUP.py && make test
```

**Then read:** DOCUMENTATION.md and plan.md

**Then code:** Start with Task 3 (expand lexer tests)

---

Good luck! 🚀
