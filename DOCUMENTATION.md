# SIMPLES Compiler - Full Documentation

## Project Overview

A complete compiler for the **SIMPLES** language (Portuguese pseudocode) that generates NASM x86 32-bit assembly targeting Linux.

**Technology Stack:**
- Language: C99
- Build: GNU Make
- Compiler: GCC
- Testing: Custom unit tests (simple assertions)
- Target: NASM ELF32 (Linux 32-bit)

---

## Language Grammar: SIMPLES

### Tokens (47 total)

**Keywords (27):**
- Control: `programa`, `inicio`, `fim`
- Types: `inteiro`, `flutuante`, `vazio`
- Conditionals: `se`, `entao`, `senao`, `fimse`
- Loops: `enquanto`, `fimenquanto`, `para`, `de`, `ate`, `passo`, `faca`, `fimpara`
- I/O: `leia`, `escreva`, `escreval`
- Operators: `e`, `ou`, `nao`, `div`
- Procedures: `procedimento`, `retorna`

**Operators (14):**
- Assignment: `<-`
- Arithmetic: `+`, `-`, `*`, `div`
- Relational: `>`, `<`, `=`, `<>`, `>=`, `<=`
- Logical: `e`, `ou`, `nao`
- Delimiters: `(`, `)`, `,`, `;`

**Literals & Identifiers (3):**
- `ID`: `[A-Za-z_][A-Za-z0-9_]*`
- `NUM_INT`: `[0-9]+`
- `NUM_FLOAT`: `[0-9]+\.[0-9]+`

### Grammar

```ebnf
<programa>        ::= "programa" ID <declaracoes> "inicio" <comandos> "fim"

<declaracoes>     ::= { <declaracao> }
<declaracao>      ::= <tipo> ID { "," ID }
<tipo>            ::= "inteiro" | "flutuante"

<comandos>        ::= { <comando> }
<comando>         ::= <atribuicao>
                    | <cmd_leia>
                    | <cmd_escreva>
                    | <cmd_se>
                    | <cmd_enquanto>
                    | <cmd_para>

<atribuicao>      ::= ID "<-" <expressao>
<cmd_leia>        ::= "leia" ID
<cmd_escreva>     ::= ("escreva" | "escreval") <expressao>

<cmd_se>          ::= "se" <expressao> "entao" <comandos>
                       [ "senao" <comandos> ] "fimse"

<cmd_enquanto>    ::= "enquanto" <expressao> "faca" <comandos> "fimenquanto"

<cmd_para>        ::= "para" ID "de" <expressao> "ate" <expressao>
                       "passo" <expressao> "faca" <comandos> "fimpara"

<expressao>       ::= <expr_logica>
<expr_logica>     ::= <expr_relacional> { ("e" | "ou") <expr_relacional> }
<expr_relacional> ::= <expr_aditiva> [ <op_relacional> <expr_aditiva> ]
<op_relacional>   ::= ">" | "<" | "=" | "<>" | ">=" | "<="
<expr_aditiva>    ::= <expr_mult> { ("+" | "-") <expr_mult> }
<expr_mult>       ::= <fator> { ("*" | "div") <fator> }
<fator>           ::= ID | NUM_INT | NUM_FLOAT
                    | "(" <expressao> ")"
                    | "nao" <fator>
                    | "-" <fator>
```

---

## Compiler Architecture

```
Source Code (SIMPLES)
    ↓
[Lexer]          → Tokenize
    ↓
Token Stream
    ↓
[Parser]         → Build AST
    ↓
Abstract Syntax Tree (AST)
    ↓
[Semantic Analyzer] → Type check, resolve symbols
    ↓
Annotated AST + Symbol Table
    ↓
[Code Generator] → Generate NASM assembly
    ↓
Assembly Output (NASM x86-32)
```

### Phases

**Phase 1: Lexer (Lexical Analysis)**
- Input: Source file text
- Output: Token stream
- Files: `lexer.c`, `lexer.h`, `token.h`

**Phase 2: Parser (Syntax Analysis)**
- Input: Token stream
- Output: Abstract Syntax Tree (AST)
- Files: `parser.c`, `parser.h`, `ast.c`, `ast.h`

**Phase 3: Semantic Analyzer**
- Input: AST
- Output: Annotated AST + symbol table
- Tasks: Type checking, variable validation, scope management
- Files: `semantic.c`, `semantic.h`, `symbol.c`, `symbol.h`

**Phase 4: Code Generator**
- Input: Annotated AST
- Output: NASM assembly
- Tasks: Expression evaluation, instruction selection, label management
- Files: `codegen.c`, `codegen.h`

**Entry Point:**
- `main.c` - CLI interface

---

## File Structure

```
compilador_cop/
├── src/
│   ├── token.h              # Token definitions (enum, structs)
│   ├── lexer.c / lexer.h    # Lexical analysis
│   ├── ast.c / ast.h        # AST node definitions
│   ├── parser.c / parser.h  # Syntax analysis
│   ├── symbol.c / symbol.h  # Symbol table management
│   ├── semantic.c / semantic.h  # Semantic analysis
│   ├── codegen.c / codegen.h    # Code generation
│   └── main.c               # CLI entry point
├── tests/
│   ├── test_lexer.c
│   ├── test_parser.c
│   ├── test_semantic.c
│   └── test_codegen.c
├── examples/
│   ├── assignment.simples
│   ├── conditional.simples
│   ├── loop_while.simples
│   ├── loop_for.simples
│   ├── fatorial.simples
│   └── fibonacci.simples
├── Makefile
└── .gitignore
```

---

## Build System (Makefile)

```bash
# Compile compiler
make all              # Build simplesc binary

# Run tests
make test             # Compile and run all tests

# Clean
make clean            # Remove all generated files
```

---

## Implementation Phases & Tasks

### Phase 1: Lexer (3 tasks)
- [x] Task 1: Project setup & token definitions
- [ ] Task 2: Lexer core engine implementation
- [ ] Task 3: Complete lexer test suite

### Phase 2: Parser & AST (7 tasks)
- [ ] Task 4: AST node structures
- [ ] Task 5: Symbol table
- [ ] Task 6: Parser core (recursive descent)
- [ ] Task 7: Parser - expression parsing
- [ ] Task 8: Parser - statement parsing
- [ ] Task 9: Parser - full program structure
- [ ] Task 10: Parser test suite

### Phase 3: Semantic Analysis (2 tasks)
- [ ] Task 11: Semantic analyzer core
- [ ] Task 12: Semantic analyzer test suite

### Phase 4: Code Generation (8 tasks)
- [ ] Task 13: Code generator - expressions
- [ ] Task 14: Code generator - statements
- [ ] Task 15: Code generator - I/O operations
- [ ] Task 16: Code generator - complete implementation
- [ ] Task 17: Code generator test suite
- [ ] Task 18: Main CLI & integration
- [ ] Task 19: End-to-end integration tests
- [ ] Task 20: Example programs & validation

---

## Example Program

**Input (SIMPLES):**
```simples
programa teste
  inteiro n, fat, contador
inicio
  leia n
  fat <- 1
  contador <- 1
  enquanto contador <= n faca
    fat <- fat * contador
    contador <- contador + 1
  fimenquanto
  escreva fat
fim
```

**Output (NASM x86-32):**
```nasm
section .bss
    n      resd 1
    fat    resd 1
    contador resd 1
    buf    resb 12

section .text
    global _start

_start:
    ; leia n
    mov eax, 3
    mov ebx, 0
    mov ecx, buf
    mov edx, 12
    int 0x80
    ; ... parse and store n ...
    
    ; fat := 1
    mov dword [fat], 1
    
    ; contador := 1
    mov dword [contador], 1

.loop_0:
    ; enquanto contador <= n
    mov eax, [contador]
    cmp eax, [n]
    jg .fim_loop_0
    
    ; fat := fat * contador
    mov eax, [fat]
    imul eax, [contador]
    mov [fat], eax
    
    ; contador := contador + 1
    mov eax, [contador]
    inc eax
    mov [contador], eax
    
    jmp .loop_0

.fim_loop_0:
    ; escreva fat
    ; ... output fat ...
    
    ; exit(0)
    mov eax, 1
    xor ebx, ebx
    int 0x80
```

---

## Testing Strategy

**Test-Driven Development (TDD):**
1. Write failing test
2. Implement minimal code
3. Make test pass
4. Refactor if needed

**Test Levels:**
- Unit tests for each module
- Integration tests for phase transitions
- End-to-end tests for complete compilation

**Test Framework:**
- Custom simple assertions (ASSERT_EQUAL, ASSERT_NOT_NULL)
- No external dependencies

---

## Development Guidelines

1. **One file, one responsibility** - Each .c file has clear purpose
2. **Complete memory management** - No leaks, proper malloc/free
3. **Error handling** - Meaningful error messages with line/column
4. **Code clarity** - Variable names, minimal comments where needed
5. **No feature creep** - Stick to specified scope for v1.0

---

## Out of Scope (v1.0)

- Procedures (procedimento/retorna)
- Arrays and strings
- Floating-point code generation
- Code optimization
- Windows support (PE/COFF)
- Error recovery (panic mode)

---

## Appendix: Syscalls Reference

| Operation | EAX | EBX | ECX | EDX |
|-----------|-----|-----|-----|-----|
| sys_exit | 1 | exit code | - | - |
| sys_read | 3 | 0 (stdin) | buffer | size |
| sys_write | 4 | 1 (stdout) | buffer | size |

All invoked via `int 0x80` on 32-bit Linux.

---

Last Updated: Compiler Implementation Day 1
Status: Lexer phase in progress
