# 🚀 PHASE 2: PARSER IMPLEMENTATION - STARTING NOW

## Task 4: AST Node Structures

**Objective:** Create the Abstract Syntax Tree (AST) data structures that the parser will build.

### What Gets Created

**File: src/ast.h** (~150 lines)
- 16 AST node types (PROGRAM, DECLARATION, ASSIGNMENT, IF, WHILE, FOR, BINOP, UNOP, etc.)
- 4 data types (TYPE_INT, TYPE_FLOAT, TYPE_VOID, TYPE_UNKNOWN)
- Large union with struct for each node type
- Factory function declarations

**File: src/ast.c** (~200 lines)
- Factory functions to create each node type
- `ast_add_child()` to add nodes to lists
- `ast_free()` to deallocate nodes
- `ast_print()` to debug-print the tree

### Node Types

```
NODE_PROGRAM       - programa name decl* inicio cmds fim
NODE_DECLARATION   - type name
NODE_DECLARATIONS  - list of declarations
NODE_COMMANDS      - list of commands
NODE_ASSIGNMENT    - id <- expr
NODE_READ          - leia id
NODE_WRITE         - escreva expr
NODE_WRITELN       - escreval expr
NODE_IF            - se expr entao cmds senao cmds fimse
NODE_WHILE         - enquanto expr faca cmds fimenquanto
NODE_FOR           - para id de expr ate expr passo expr faca cmds fimpara
NODE_BINOP         - left op right (+, -, *, >, <, =, etc.)
NODE_UNOP          - op operand (-, nao)
NODE_ID            - identifier reference
NODE_NUM_INT       - integer literal
NODE_NUM_FLOAT     - float literal
```

### Time Estimate: 30 minutes

### Implementation Status
- [ ] Create src/ast.h with node types and union
- [ ] Implement factory functions in src/ast.c
- [ ] Implement utilities (add_child, free, print)
- [ ] Test with simple AST creation

---

## Next Steps After Task 4

**Task 5:** Symbol Table (20 min)
- Track variable declarations
- Scope management

**Task 6-9:** Parser Implementation (2+ hours)
- Recursive descent parser
- Expression parsing
- Statement parsing
- Program parsing

**Task 10:** Parser Tests (20 min)

---

## How to Proceed

### Option A: Let me dispatch a subagent
I can create a background agent to implement Task 4 automatically.

### Option B: Work together
I guide you through the implementation step by step.

### Option C: You implement
I provide the plan, you write the code.

**Which would you prefer?** (A, B, or C)

Or just say "continue" and I'll start with Option A (subagent).
