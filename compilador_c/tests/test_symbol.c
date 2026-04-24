#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/symbol.h"

/* Test counter */
static int test_count = 0;
static int test_passed = 0;

void assert_true(int condition, const char *message) {
    test_count++;
    if (condition) {
        printf("  ✓ %s\n", message);
        test_passed++;
    } else {
        printf("  ✗ %s\n", message);
    }
}

void assert_equal_int(int actual, int expected, const char *message) {
    test_count++;
    if (actual == expected) {
        printf("  ✓ %s (got %d)\n", message, actual);
        test_passed++;
    } else {
        printf("  ✗ %s (expected %d, got %d)\n", message, expected, actual);
    }
}

void assert_null(void *ptr, const char *message) {
    test_count++;
    if (ptr == NULL) {
        printf("  ✓ %s\n", message);
        test_passed++;
    } else {
        printf("  ✗ %s\n", message);
    }
}

void assert_not_null(void *ptr, const char *message) {
    test_count++;
    if (ptr != NULL) {
        printf("  ✓ %s\n", message);
        test_passed++;
    } else {
        printf("  ✗ %s\n", message);
    }
}

void test_create_symbol_table(void) {
    printf("\n=== Test: Create Symbol Table ===\n");
    
    SymbolTable *table = symbol_table_create();
    assert_not_null(table, "Symbol table created");
    assert_not_null(table->symbols, "Symbols array allocated");
    assert_equal_int(table->count, 0, "Initial count is 0");
    assert_equal_int(table->capacity, 100, "Initial capacity is 100");
    assert_equal_int(table->scope_level, 0, "Initial scope level is 0 (global)");
    
    symbol_table_free(table);
    printf("Symbol table freed successfully\n");
}

void test_add_symbols(void) {
    printf("\n=== Test: Add Symbols ===\n");
    
    SymbolTable *table = symbol_table_create();
    assert_not_null(table, "Symbol table created");
    
    /* Add first symbol */
    int result = symbol_table_add(table, "x", TYPE_INT, 1, 5);
    assert_equal_int(result, 0, "Adding 'x' returns 0");
    assert_equal_int(table->count, 1, "Count is 1 after adding one symbol");
    
    /* Add second symbol */
    result = symbol_table_add(table, "y", TYPE_FLOAT, 2, 10);
    assert_equal_int(result, 0, "Adding 'y' returns 0");
    assert_equal_int(table->count, 2, "Count is 2 after adding two symbols");
    
    /* Add third symbol */
    result = symbol_table_add(table, "z", TYPE_INT, 3, 15);
    assert_equal_int(result, 0, "Adding 'z' returns 0");
    assert_equal_int(table->count, 3, "Count is 3 after adding three symbols");
    
    symbol_table_free(table);
    printf("Symbols added successfully\n");
}

void test_lookup_symbols(void) {
    printf("\n=== Test: Lookup Symbols ===\n");
    
    SymbolTable *table = symbol_table_create();
    
    /* Add symbols */
    symbol_table_add(table, "x", TYPE_INT, 1, 5);
    symbol_table_add(table, "y", TYPE_FLOAT, 2, 10);
    symbol_table_add(table, "z", TYPE_INT, 3, 15);
    
    /* Look up existing symbols */
    Symbol *sym = symbol_table_lookup(table, "x");
    assert_not_null(sym, "Symbol 'x' found");
    if (sym != NULL) {
        assert_equal_int(sym->type, TYPE_INT, "Symbol 'x' has type INT");
        assert_equal_int(sym->line, 1, "Symbol 'x' at line 1");
        assert_equal_int(sym->column, 5, "Symbol 'x' at column 5");
    }
    
    sym = symbol_table_lookup(table, "y");
    assert_not_null(sym, "Symbol 'y' found");
    if (sym != NULL) {
        assert_equal_int(sym->type, TYPE_FLOAT, "Symbol 'y' has type FLOAT");
    }
    
    /* Look up non-existing symbol */
    sym = symbol_table_lookup(table, "undefined");
    assert_null(sym, "Non-existing symbol returns NULL");
    
    symbol_table_free(table);
    printf("Lookups successful\n");
}

void test_duplicate_detection(void) {
    printf("\n=== Test: Duplicate Detection ===\n");
    
    SymbolTable *table = symbol_table_create();
    
    /* Add first declaration of 'x' */
    int result = symbol_table_add(table, "x", TYPE_INT, 1, 5);
    assert_equal_int(result, 0, "First 'x' added successfully");
    
    /* Try to add duplicate 'x' at same scope */
    result = symbol_table_add(table, "x", TYPE_FLOAT, 2, 10);
    assert_equal_int(result, -1, "Duplicate 'x' at same scope returns -1");
    assert_equal_int(table->count, 1, "Count remains 1 (duplicate not added)");
    
    /* Add different variable */
    result = symbol_table_add(table, "y", TYPE_INT, 3, 15);
    assert_equal_int(result, 0, "Different variable 'y' added successfully");
    
    symbol_table_free(table);
    printf("Duplicate detection working\n");
}

void test_scope_entry_exit(void) {
    printf("\n=== Test: Scope Entry and Exit ===\n");
    
    SymbolTable *table = symbol_table_create();
    
    /* Global scope (0) */
    assert_equal_int(table->scope_level, 0, "Initial scope level is 0");
    symbol_table_add(table, "global_x", TYPE_INT, 1, 5);
    assert_equal_int(table->count, 1, "Global symbol added");
    
    /* Enter scope 1 */
    symbol_table_enter_scope(table);
    assert_equal_int(table->scope_level, 1, "Scope level is 1 after enter");
    symbol_table_add(table, "local_y", TYPE_FLOAT, 2, 10);
    assert_equal_int(table->count, 2, "Local symbol added at scope 1");
    
    /* Can add duplicate name in different scope */
    int result = symbol_table_add(table, "global_x", TYPE_FLOAT, 3, 15);
    assert_equal_int(result, 0, "Can add 'global_x' again at scope 1");
    assert_equal_int(table->count, 3, "Now have 3 symbols");
    
    /* Enter scope 2 */
    symbol_table_enter_scope(table);
    assert_equal_int(table->scope_level, 2, "Scope level is 2 after second enter");
    symbol_table_add(table, "inner_z", TYPE_INT, 4, 20);
    assert_equal_int(table->count, 4, "Inner symbol added at scope 2");
    
    /* Exit scope 2 - should remove inner_z */
    symbol_table_exit_scope(table);
    assert_equal_int(table->scope_level, 1, "Scope level is 1 after exit");
    assert_equal_int(table->count, 3, "Count is 3 after exiting scope 2");
    
    /* Verify inner_z is gone */
    Symbol *sym = symbol_table_lookup(table, "inner_z");
    assert_null(sym, "Symbol 'inner_z' removed after exiting scope");
    
    /* Exit scope 1 - should remove symbols added at scope 1 */
    symbol_table_exit_scope(table);
    assert_equal_int(table->scope_level, 0, "Scope level is 0 after second exit");
    assert_equal_int(table->count, 1, "Count is 1 after exiting scope 1");
    
    /* Only global symbol remains */
    sym = symbol_table_lookup(table, "global_x");
    assert_not_null(sym, "Global symbol 'global_x' still exists");
    if (sym != NULL) {
        assert_equal_int(sym->scope_level, 0, "Symbol is at global scope");
    }
    
    symbol_table_free(table);
    printf("Scope management working correctly\n");
}

void test_symbol_exists(void) {
    printf("\n=== Test: Symbol Exists Check ===\n");
    
    SymbolTable *table = symbol_table_create();
    
    /* Add some symbols */
    symbol_table_add(table, "x", TYPE_INT, 1, 5);
    symbol_table_add(table, "y", TYPE_FLOAT, 2, 10);
    
    /* Check existence */
    assert_equal_int(symbol_table_exists(table, "x"), 1, "Symbol 'x' exists");
    assert_equal_int(symbol_table_exists(table, "y"), 1, "Symbol 'y' exists");
    assert_equal_int(symbol_table_exists(table, "z"), 0, "Symbol 'z' does not exist");
    assert_equal_int(symbol_table_exists(table, "undefined"), 0, "Symbol 'undefined' does not exist");
    
    symbol_table_free(table);
    printf("Symbol existence check working\n");
}

void test_lookup_with_shadowing(void) {
    printf("\n=== Test: Lookup with Variable Shadowing ===\n");
    
    SymbolTable *table = symbol_table_create();
    
    /* Add x at global scope */
    symbol_table_add(table, "x", TYPE_INT, 1, 5);
    Symbol *sym = symbol_table_lookup(table, "x");
    assert_not_null(sym, "Global 'x' found");
    if (sym != NULL) {
        assert_equal_int(sym->type, TYPE_INT, "Global 'x' is INT");
        assert_equal_int(sym->line, 1, "Global 'x' at line 1");
    }
    
    /* Enter scope and add x again (shadowing) */
    symbol_table_enter_scope(table);
    symbol_table_add(table, "x", TYPE_FLOAT, 10, 8);
    sym = symbol_table_lookup(table, "x");
    assert_not_null(sym, "Local 'x' found");
    if (sym != NULL) {
        assert_equal_int(sym->type, TYPE_FLOAT, "Local 'x' is FLOAT (shadows global)");
        assert_equal_int(sym->line, 10, "Local 'x' at line 10");
    }
    
    /* Exit scope - should find global 'x' again */
    symbol_table_exit_scope(table);
    sym = symbol_table_lookup(table, "x");
    assert_not_null(sym, "Global 'x' found again after scope exit");
    if (sym != NULL) {
        assert_equal_int(sym->type, TYPE_INT, "Global 'x' is INT again");
        assert_equal_int(sym->line, 1, "Global 'x' still at line 1");
    }
    
    symbol_table_free(table);
    printf("Variable shadowing working correctly\n");
}

void test_capacity_expansion(void) {
    printf("\n=== Test: Capacity Expansion ===\n");
    
    SymbolTable *table = symbol_table_create();
    assert_equal_int(table->capacity, 100, "Initial capacity is 100");
    
    /* Add more than initial capacity */
    for (int i = 0; i < 120; i++) {
        char name[20];
        snprintf(name, sizeof(name), "var_%d", i);
        int result = symbol_table_add(table, name, TYPE_INT, i + 1, i + 5);
        assert_equal_int(result, 0, "Adding variable does not fail");
    }
    
    assert_equal_int(table->count, 120, "All 120 variables added");
    assert_true(table->capacity >= 120, "Capacity expanded to accommodate all variables");
    
    /* Verify all symbols are accessible */
    Symbol *sym = symbol_table_lookup(table, "var_50");
    assert_not_null(sym, "Can lookup symbol from middle of array");
    
    sym = symbol_table_lookup(table, "var_119");
    assert_not_null(sym, "Can lookup last symbol");
    
    symbol_table_free(table);
    printf("Capacity expansion working\n");
}

int main(void) {
    printf("====== Symbol Table Tests ======\n");
    
    test_create_symbol_table();
    test_add_symbols();
    test_lookup_symbols();
    test_duplicate_detection();
    test_scope_entry_exit();
    test_symbol_exists();
    test_lookup_with_shadowing();
    test_capacity_expansion();
    
    printf("\n====== Test Summary ======\n");
    printf("Tests passed: %d / %d\n", test_passed, test_count);
    
    if (test_passed == test_count) {
        printf("✓ All tests passed!\n");
        return 0;
    } else {
        printf("✗ Some tests failed\n");
        return 1;
    }
}
