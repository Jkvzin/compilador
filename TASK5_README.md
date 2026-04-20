# Task 5: Symbol Table Implementation

## Overview

This implementation provides the symbol table for the SIMPLES compiler's semantic analysis phase. The symbol table is the foundation for:
- Variable declaration tracking
- Type checking
- Scope management for nested procedures
- Error reporting with line/column information

## Files Implemented

### 1. `src/symbol.h` (48 lines)
Header file defining the symbol table interface.

**Structs:**
```c
typedef struct {
    char *name;           // Variable name
    DataType type;        // Type (from ast.h: INT, FLOAT, VOID, UNKNOWN)
    int line;             // Line number for error reporting
    int column;           // Column number for error reporting
    int scope_level;      // Scope level (0=global, >0=nested)
} Symbol;

typedef struct {
    Symbol *symbols;      // Dynamic array of symbols
    size_t count;         // Current number of symbols
    size_t capacity;      // Allocated capacity
    int scope_level;      // Current scope level
} SymbolTable;
```

**Functions:**
- `symbol_table_create()` - Create new symbol table
- `symbol_table_free()` - Free all resources
- `symbol_table_add()` - Add symbol (detects duplicates)
- `symbol_table_lookup()` - Find symbol by name
- `symbol_table_exists()` - Check if symbol exists
- `symbol_table_enter_scope()` - Enter nested scope
- `symbol_table_exit_scope()` - Exit scope and cleanup

### 2. `src/symbol.c` (148 lines)
Complete implementation of the symbol table.

**Key Features:**
- Dynamic array with automatic expansion (doubles capacity)
- Duplicate detection only at current scope level
- Lookup searches from end (enables variable shadowing)
- Proper memory management with no leaks
- Scope-aware symbol management

**Implementation Details:**

**symbol_table_create():**
- Allocates SymbolTable struct
- Allocates symbols array (capacity 100)
- Initializes count=0, scope_level=0 (global)

**symbol_table_add():**
- Checks for duplicates at current scope only
- Returns -1 on duplicate or allocation failure
- Reallocs array when capacity exceeded
- Allocates and copies symbol name
- Returns 0 on success

**symbol_table_lookup():**
- Searches backwards (most recent first)
- Finds symbol if scope_level <= current_scope_level
- Enables variable shadowing in nested scopes
- Returns NULL if not found

**symbol_table_enter_scope():**
- Increments scope_level
- Allows adding symbols to new scope

**symbol_table_exit_scope():**
- Removes all symbols at current scope level
- Frees symbol names properly
- Compacts array efficiently
- Decrements scope_level

### 3. `tests/test_symbol.c` (303 lines)
Comprehensive test suite with 8 test functions and 61 assertions.

**Test Functions:**
1. **test_create_symbol_table()** - Creation and initialization (5 asserts)
2. **test_add_symbols()** - Adding multiple symbols (7 asserts)
3. **test_lookup_symbols()** - Finding symbols and checking types (9 asserts)
4. **test_duplicate_detection()** - Preventing duplicates (6 asserts)
5. **test_scope_entry_exit()** - Scope management (16 asserts)
6. **test_symbol_exists()** - Existence checking (4 asserts)
7. **test_lookup_with_shadowing()** - Variable shadowing (9 asserts)
8. **test_capacity_expansion()** - Dynamic array growth (5 asserts)

**Coverage:**
- All 7 functions tested
- Memory cleanup verified
- Scope level tracking
- Duplicate detection
- Variable shadowing across scopes
- Dynamic capacity expansion with 120+ symbols

## Usage Example

```c
// Create symbol table
SymbolTable *table = symbol_table_create();

// Add symbols at global scope (level 0)
symbol_table_add(table, "x", TYPE_INT, 1, 5);
symbol_table_add(table, "y", TYPE_FLOAT, 2, 10);

// Look up a symbol
Symbol *sym = symbol_table_lookup(table, "x");
if (sym != NULL) {
    printf("Variable %s is type %d at line %d\n", sym->name, sym->type, sym->line);
}

// Enter nested scope (for procedures)
symbol_table_enter_scope(table);
symbol_table_add(table, "local_z", TYPE_INT, 5, 8);

// Variables in global scope still accessible
Symbol *global_x = symbol_table_lookup(table, "x");  // Found (scope_level 0 <= 1)

// Exit nested scope - local_z is removed
symbol_table_exit_scope(table);

// local_z no longer accessible
Symbol *not_found = symbol_table_lookup(table, "local_z");  // NULL

// Clean up
symbol_table_free(table);
```

## Technical Details

### Scope Management
- **Level 0:** Global scope (main program)
- **Level 1+:** Nested scopes (for procedures, if statements, loops, etc.)
- Symbols are visible if declared at scope_level <= current_scope_level

### Duplicate Detection
- Checked only at current scope level
- Same variable can exist in different scopes (shadowing)
- Lookup finds most recent declaration (searches backwards)

### Memory Management
- Initial capacity: 100 symbols
- Capacity doubles when exceeded (100 → 200 → 400 → ...)
- All symbol names properly allocated and freed
- No memory leaks in any operation

### Time Complexity
- `symbol_table_add()`: O(n) - checks for duplicates
- `symbol_table_lookup()`: O(n) - linear search from end
- `symbol_table_enter_scope()`: O(1)
- `symbol_table_exit_scope()`: O(n) - compacts array

## Integration with Compiler

The symbol table is used by:

1. **Lexer** - No direct use (works on tokens)
2. **Parser** - No direct use (builds AST)
3. **Semantic Analyzer** - Main user
   - Processes declarations from AST
   - Adds symbols when declaring variables
   - Checks for duplicate declarations
   - Type checks expressions

4. **Code Generator** - Looks up symbol info for code generation
   - Gets variable types for type-specific operations
   - Uses line/column for error messages

## Code Quality

✅ **C99 Standard** - Follows C99 conventions
✅ **Memory Safe** - No leaks, proper allocation/deallocation
✅ **Well Commented** - Clear documentation of complex logic
✅ **Error Handling** - Returns proper error codes
✅ **Tested** - 61 assertions covering all functionality

## Building and Testing

### Compilation
```bash
make clean
make test
```

This will:
1. Compile all source files in `src/`
2. Compile all test files in `tests/`
3. Link with Unity test framework
4. Run tests and show results

### Running Tests Only
```bash
make test
```

Output shows:
- Test function names
- Individual assertion results (✓/✗)
- Total pass/fail count

## Git Commit

Files committed:
- `src/symbol.h` - Symbol table interface
- `src/symbol.c` - Symbol table implementation
- `tests/test_symbol.c` - Test suite

```bash
git add src/symbol.h src/symbol.c tests/test_symbol.c
git commit -m "feat: implement symbol table

- Track variable declarations and types
- Support nested scopes (foundation for procedures)
- Detect duplicate declarations at current scope
- Provide lookup by name with scope awareness

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"
```

## Requirements Met

✅ Symbol struct with all required fields
✅ SymbolTable struct with dynamic array
✅ 7 functions fully implemented
✅ ~50 lines in symbol.h
✅ ~150 lines in symbol.c
✅ Comprehensive test suite (8 functions, 61 assertions)
✅ C99 standard compliance
✅ Proper error handling
✅ No memory leaks
✅ Git commit with proper message

## Next Steps

The symbol table implementation provides the foundation for:

1. **Task 6: Semantic Analysis** - Use symbol table for:
   - Variable declaration checking
   - Duplicate detection
   - Type validation
   - Scope validation

2. **Task 7: Code Generation** - Use symbol table for:
   - Variable storage allocation
   - Type-specific code generation
   - Error reporting with line/column info

3. **Future: Procedure Support** - Enhanced symbol table for:
   - Procedure parameters
   - Return types
   - Local scopes for procedures
