# Task 5: Symbol Table - In Progress

A subagent is currently implementing the Symbol Table component.

## What's Being Built

**src/symbol.h** - Symbol table interface
- Symbol struct: name, type, line, column, scope_level
- SymbolTable struct: symbols array, count, capacity, scope_level
- 7 functions for management

**src/symbol.c** - Symbol table implementation
- Create/free tables
- Add symbols with duplicate detection
- Lookup by name with scope awareness
- Scope management (enter/exit)

**tests/test_symbol.c** - Comprehensive tests
- Test all functions
- Test scope management
- Verify memory cleanup

## Current Status

🔄 **Subagent working... (ETA ~5-10 minutes)**

This involves:
1. Creating the header file with struct definitions
2. Implementing all functions
3. Writing test cases
4. Verifying compilation
5. Creating git commit

## What It Does

The symbol table:
- **Tracks variables** - stores name, type, line, column
- **Detects duplicates** - prevents redeclaration at same scope
- **Manages scope** - supports nested scopes (for future procedures)
- **Provides lookup** - find variables by name, respecting scope rules

## Time Estimate

Total: ~20 minutes (including tests and verification)

## Architecture

```
SymbolTable
  ├── Symbol[] (dynamic array)
  │   ├── name
  │   ├── type (int, float, void, unknown)
  │   ├── line/column (for errors)
  │   └── scope_level
  └── scope_level (current nesting depth)
```

Functions exposed:
- symbol_table_create() → allocate new table
- symbol_table_add() → add variable, check duplicates
- symbol_table_lookup() → find variable in scope
- symbol_table_enter_scope() → begin nested block
- symbol_table_exit_scope() → end nested block, cleanup
- symbol_table_free() → deallocate

---

**Check back in a few minutes for results!**
