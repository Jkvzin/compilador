#ifndef SYMBOL_H
#define SYMBOL_H

#include <stddef.h>
#include "ast.h"

/* Symbol table entry */
typedef struct {
    char *name;           /* Variable name */
    DataType type;        /* Type (INT, FLOAT, VOID, UNKNOWN) */
    int line;             /* Line number where declared (for error reporting) */
    int column;           /* Column number where declared (for error reporting) */
    int scope_level;      /* Scope level (0=global, >0=nested) */
} Symbol;

/* Symbol table structure */
typedef struct {
    Symbol *symbols;      /* Dynamic array of symbols */
    size_t count;         /* Current number of symbols */
    size_t capacity;      /* Allocated capacity */
    int scope_level;      /* Current scope level */
} SymbolTable;

/* Create a new symbol table */
SymbolTable *symbol_table_create(void);

/* Free a symbol table and all its resources */
void symbol_table_free(SymbolTable *table);

/* Add a symbol to the table
 * Returns 0 on success, -1 if duplicate at current scope */
int symbol_table_add(SymbolTable *table, const char *name, DataType type, int line, int column);

/* Look up a symbol by name
 * Returns pointer to symbol if found, NULL otherwise */
Symbol *symbol_table_lookup(SymbolTable *table, const char *name);

/* Check if a symbol exists
 * Returns 1 if found, 0 if not found */
int symbol_table_exists(SymbolTable *table, const char *name);

/* Enter a new scope (increment scope level) */
void symbol_table_enter_scope(SymbolTable *table);

/* Exit current scope (remove symbols at this level and decrement scope level) */
void symbol_table_exit_scope(SymbolTable *table);

#endif /* SYMBOL_H */
