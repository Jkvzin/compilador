#include <stdlib.h>
#include <string.h>
#include "symbol.h"

/* Create a new symbol table with initial capacity */
SymbolTable *symbol_table_create(void) {
    SymbolTable *table = (SymbolTable *)malloc(sizeof(SymbolTable));
    if (table == NULL) {
        return NULL;
    }
    
    table->symbols = (Symbol *)malloc(100 * sizeof(Symbol));
    if (table->symbols == NULL) {
        free(table);
        return NULL;
    }
    
    table->count = 0;
    table->capacity = 100;
    table->scope_level = 0;
    
    return table;
}

/* Free symbol table and all resources */
void symbol_table_free(SymbolTable *table) {
    if (table == NULL) {
        return;
    }
    
    /* Free all symbol names */
    for (size_t i = 0; i < table->count; i++) {
        if (table->symbols[i].name != NULL) {
            free(table->symbols[i].name);
        }
    }
    
    /* Free symbols array */
    free(table->symbols);
    
    /* Free table itself */
    free(table);
}

/* Add a symbol to the table
 * Returns 0 on success, -1 if duplicate at current scope */
int symbol_table_add(SymbolTable *table, const char *name, DataType type, int line, int column) {
    if (table == NULL || name == NULL) {
        return -1;
    }
    
    /* Check for duplicate at current scope level */
    for (size_t i = 0; i < table->count; i++) {
        if (table->symbols[i].scope_level == table->scope_level &&
            strcmp(table->symbols[i].name, name) == 0) {
            return -1;  /* Duplicate found at current scope */
        }
    }
    
    /* Expand capacity if needed */
    if (table->count >= table->capacity) {
        size_t new_capacity = table->capacity * 2;
        Symbol *new_symbols = (Symbol *)realloc(table->symbols, new_capacity * sizeof(Symbol));
        if (new_symbols == NULL) {
            return -1;
        }
        table->symbols = new_symbols;
        table->capacity = new_capacity;
    }
    
    /* Add new symbol */
    Symbol *sym = &table->symbols[table->count];
    sym->name = (char *)malloc(strlen(name) + 1);
    if (sym->name == NULL) {
        return -1;
    }
    
    strcpy(sym->name, name);
    sym->type = type;
    sym->line = line;
    sym->column = column;
    sym->scope_level = table->scope_level;
    
    table->count++;
    return 0;
}

/* Look up a symbol by name
 * Search from end (most recent first)
 * Returns symbol if found at scope <= current scope, NULL if not found */
Symbol *symbol_table_lookup(SymbolTable *table, const char *name) {
    if (table == NULL || name == NULL) {
        return NULL;
    }
    
    /* Search from end (most recent first) */
    for (int i = (int)table->count - 1; i >= 0; i--) {
        if (table->symbols[i].scope_level <= table->scope_level &&
            strcmp(table->symbols[i].name, name) == 0) {
            return &table->symbols[i];
        }
    }
    
    return NULL;
}

/* Check if a symbol exists
 * Returns 1 if found, 0 if not found */
int symbol_table_exists(SymbolTable *table, const char *name) {
    return symbol_table_lookup(table, name) != NULL ? 1 : 0;
}

/* Enter a new scope (increment scope level) */
void symbol_table_enter_scope(SymbolTable *table) {
    if (table != NULL) {
        table->scope_level++;
    }
}

/* Exit current scope
 * Remove symbols at current scope and decrement scope level */
void symbol_table_exit_scope(SymbolTable *table) {
    if (table == NULL || table->scope_level == 0) {
        return;
    }
    
    /* Remove symbols at current scope by compacting array */
    size_t write_pos = 0;
    for (size_t read_pos = 0; read_pos < table->count; read_pos++) {
        if (table->symbols[read_pos].scope_level != table->scope_level) {
            /* Keep this symbol */
            if (write_pos != read_pos) {
                /* Copy symbol to new position */
                free(table->symbols[write_pos].name);
                table->symbols[write_pos] = table->symbols[read_pos];
                table->symbols[read_pos].name = NULL;  /* Prevent double-free */
            }
            write_pos++;
        } else {
            /* Remove this symbol - free its name */
            free(table->symbols[read_pos].name);
            table->symbols[read_pos].name = NULL;
        }
    }
    
    table->count = write_pos;
    table->scope_level--;
}
