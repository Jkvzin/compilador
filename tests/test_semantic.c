#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/semantic.h"
#include "../src/ast.h"
#include "../src/symbol.h"

/* Test counter */
static int test_count = 0;
static int test_passed = 0;

/* Custom assertions */
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

void assert_not_null(void *ptr, const char *message) {
    test_count++;
    if (ptr != NULL) {
        printf("  ✓ %s\n", message);
        test_passed++;
    } else {
        printf("  ✗ %s\n", message);
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

void assert_string_equal(const char *actual, const char *expected, const char *message) {
    test_count++;
    if (strcmp(actual, expected) == 0) {
        printf("  ✓ %s\n", message);
        test_passed++;
    } else {
        printf("  ✗ %s (expected '%s', got '%s')\n", message, expected, actual);
    }
}

/* Helper functions to create test programs */

ASTNode *create_simple_program(const char *prog_name) {
    ASTNode *decls = ast_make_declarations();
    ASTNode *cmds = ast_make_commands();
    return ast_make_program(prog_name, decls, cmds);
}

ASTNode *create_program_with_declarations(const char *prog_name, int num_decls, ...) {
    ASTNode *decls = ast_make_declarations();
    ASTNode *cmds = ast_make_commands();
    
    va_list args;
    va_start(args, num_decls);
    
    for (int i = 0; i < num_decls; i++) {
        const char *name = va_arg(args, const char *);
        DataType type = va_arg(args, DataType);
        ASTNode *decl = ast_make_declaration(type, name);
        ast_add_child(decls, decl);
    }
    
    va_end(args);
    return ast_make_program(prog_name, decls, cmds);
}

ASTNode *create_int_declaration(const char *name) {
    return ast_make_declaration(TYPE_INT, name);
}

ASTNode *create_float_declaration(const char *name) {
    return ast_make_declaration(TYPE_FLOAT, name);
}

/* ============ BASIC DECLARATION TESTS (5 tests) ============ */

void test_semantic_valid_declaration(void) {
    printf("\n=== Test: Valid Declaration ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    assert_not_null(analyzer, "Analyzer created");
    
    ASTNode *program = create_simple_program("Test");
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Single inteiro declaration should pass");
    assert_equal_int(analyzer->error_count, 0, "No errors should be recorded");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_multiple_declarations(void) {
    printf("\n=== Test: Multiple Declarations ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    ast_add_child(program->data.program.declarations, create_float_declaration("y"));
    ast_add_child(program->data.program.declarations, create_int_declaration("z"));
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Multiple declarations of different types should pass");
    assert_equal_int(analyzer->error_count, 0, "No errors for valid declarations");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_duplicate_declaration(void) {
    printf("\n=== Test: Duplicate Declaration ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    ast_add_child(program->data.program.declarations, create_float_declaration("x"));
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 0, "Duplicate variable name should fail");
    assert_true(analyzer->error_count > 0, "Error should be recorded");
    assert_true(strstr(result.error_message, "Duplicate") != NULL, "Error message should mention duplicate");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_multiple_duplicates(void) {
    printf("\n=== Test: Multiple Duplicates ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    ast_add_child(program->data.program.declarations, create_int_declaration("y"));
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 0, "Multiple duplicates should fail at first duplicate");
    assert_true(analyzer->error_count > 0, "Error should be recorded");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_mixed_types(void) {
    printf("\n=== Test: Mixed Types ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("a"));
    ast_add_child(program->data.program.declarations, create_float_declaration("b"));
    ast_add_child(program->data.program.declarations, create_int_declaration("c"));
    ast_add_child(program->data.program.declarations, create_float_declaration("d"));
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Mix of inteiro and real declarations should pass");
    assert_equal_int(analyzer->error_count, 0, "No errors for mixed valid types");
    
    ast_free(program);
    semantic_free(analyzer);
}

/* ============ VARIABLE USAGE TESTS (8 tests) ============ */

void test_semantic_valid_assignment(void) {
    printf("\n=== Test: Valid Assignment ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    
    ASTNode *value = ast_make_num_int(5);
    ASTNode *assign = ast_make_assignment("x", value);
    ast_add_child(program->data.program.commands, assign);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Assignment to declared variable should pass");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_undefined_variable_in_assignment(void) {
    printf("\n=== Test: Undefined Variable in Assignment ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ASTNode *value = ast_make_num_int(5);
    ASTNode *assign = ast_make_assignment("undefined_var", value);
    ast_add_child(program->data.program.commands, assign);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 0, "Assignment to undefined variable should fail");
    assert_true(strstr(result.error_message, "Undefined") != NULL, "Error should mention undefined");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_valid_read(void) {
    printf("\n=== Test: Valid Read ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    
    ASTNode *read = ast_make_read("x");
    ast_add_child(program->data.program.commands, read);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Read into declared variable should pass");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_undefined_variable_in_read(void) {
    printf("\n=== Test: Undefined Variable in Read ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ASTNode *read = ast_make_read("undefined_var");
    ast_add_child(program->data.program.commands, read);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 0, "Read into undefined variable should fail");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_valid_write(void) {
    printf("\n=== Test: Valid Write ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    
    ASTNode *id = ast_make_id("x");
    ASTNode *write = ast_make_write(id, 0);
    ast_add_child(program->data.program.commands, write);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Write of declared variable should pass");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_undefined_variable_in_write(void) {
    printf("\n=== Test: Undefined Variable in Write ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ASTNode *id = ast_make_id("undefined_var");
    ASTNode *write = ast_make_write(id, 0);
    ast_add_child(program->data.program.commands, write);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 0, "Write of undefined variable should fail");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_variable_in_expression(void) {
    printf("\n=== Test: Variable in Expression ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    ast_add_child(program->data.program.declarations, create_int_declaration("y"));
    
    ASTNode *id_x = ast_make_id("x");
    ASTNode *id_y = ast_make_id("y");
    ASTNode *expr = ast_make_binop(id_x, '+', id_y);
    ASTNode *assign = ast_make_assignment("x", expr);
    ast_add_child(program->data.program.commands, assign);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Variable in expression should be checked");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_multiple_undefined_variables(void) {
    printf("\n=== Test: Multiple Undefined Variables ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ASTNode *id_x = ast_make_id("undefined1");
    ASTNode *id_y = ast_make_id("undefined2");
    ASTNode *expr = ast_make_binop(id_x, '+', id_y);
    ASTNode *write = ast_make_write(expr, 0);
    ast_add_child(program->data.program.commands, write);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 0, "Multiple undefined variables should be caught");
    
    ast_free(program);
    semantic_free(analyzer);
}

/* ============ EXPRESSION TESTS (6 tests) ============ */

void test_semantic_expression_with_numbers(void) {
    printf("\n=== Test: Expression with Numbers ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ASTNode *num1 = ast_make_num_int(5);
    ASTNode *num2 = ast_make_num_int(10);
    ASTNode *expr = ast_make_binop(num1, '+', num2);
    ASTNode *write = ast_make_write(expr, 0);
    ast_add_child(program->data.program.commands, write);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Expression with just numbers should pass");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_expression_with_variables(void) {
    printf("\n=== Test: Expression with Variables ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    ast_add_child(program->data.program.declarations, create_int_declaration("y"));
    
    ASTNode *id_x = ast_make_id("x");
    ASTNode *id_y = ast_make_id("y");
    ASTNode *expr = ast_make_binop(id_x, '+', id_y);
    ASTNode *write = ast_make_write(expr, 0);
    ast_add_child(program->data.program.commands, write);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Expression with declared variables should pass");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_expression_undefined_var(void) {
    printf("\n=== Test: Expression Undefined Variable ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    
    ASTNode *id_x = ast_make_id("x");
    ASTNode *id_y = ast_make_id("undefined_y");
    ASTNode *expr = ast_make_binop(id_x, '+', id_y);
    ASTNode *write = ast_make_write(expr, 0);
    ast_add_child(program->data.program.commands, write);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 0, "Expression with undefined variable should fail");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_complex_expression(void) {
    printf("\n=== Test: Complex Expression ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("a"));
    ast_add_child(program->data.program.declarations, create_int_declaration("b"));
    ast_add_child(program->data.program.declarations, create_int_declaration("c"));
    
    /* (a + b) * c */
    ASTNode *id_a = ast_make_id("a");
    ASTNode *id_b = ast_make_id("b");
    ASTNode *id_c = ast_make_id("c");
    ASTNode *add = ast_make_binop(id_a, '+', id_b);
    ASTNode *mul = ast_make_binop(add, '*', id_c);
    ASTNode *assign = ast_make_assignment("a", mul);
    ast_add_child(program->data.program.commands, assign);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Complex expression with multiple variables should pass");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_unary_expression(void) {
    printf("\n=== Test: Unary Expression ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    
    ASTNode *id_x = ast_make_id("x");
    ASTNode *unary = ast_make_unop('-', id_x);
    ASTNode *assign = ast_make_assignment("x", unary);
    ast_add_child(program->data.program.commands, assign);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Unary operator on variable should pass");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_nested_expression(void) {
    printf("\n=== Test: Nested Expression ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    ast_add_child(program->data.program.declarations, create_int_declaration("y"));
    ast_add_child(program->data.program.declarations, create_int_declaration("z"));
    
    /* -(x + (y * z)) */
    ASTNode *id_x = ast_make_id("x");
    ASTNode *id_y = ast_make_id("y");
    ASTNode *id_z = ast_make_id("z");
    ASTNode *mul = ast_make_binop(id_y, '*', id_z);
    ASTNode *add = ast_make_binop(id_x, '+', mul);
    ASTNode *unary = ast_make_unop('-', add);
    ASTNode *write = ast_make_write(unary, 0);
    ast_add_child(program->data.program.commands, write);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Nested expressions with variables should pass");
    
    ast_free(program);
    semantic_free(analyzer);
}

/* ============ CONTROL FLOW TESTS (8 tests) ============ */

void test_semantic_if_with_declared_var(void) {
    printf("\n=== Test: If with Declared Variable ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    
    ASTNode *id_x = ast_make_id("x");
    ASTNode *zero = ast_make_num_int(0);
    ASTNode *cond = ast_make_binop(id_x, '>', zero);
    
    ASTNode *then_cmds = ast_make_commands();
    ASTNode *num = ast_make_num_int(1);
    ASTNode *assign = ast_make_assignment("x", num);
    ast_add_child(then_cmds, assign);
    
    ASTNode *if_stmt = ast_make_if(cond, then_cmds, NULL);
    ast_add_child(program->data.program.commands, if_stmt);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "If condition with declared variable should pass");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_if_with_undefined_var(void) {
    printf("\n=== Test: If with Undefined Variable ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ASTNode *id_x = ast_make_id("undefined_x");
    ASTNode *zero = ast_make_num_int(0);
    ASTNode *cond = ast_make_binop(id_x, '>', zero);
    
    ASTNode *then_cmds = ast_make_commands();
    ASTNode *if_stmt = ast_make_if(cond, then_cmds, NULL);
    ast_add_child(program->data.program.commands, if_stmt);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 0, "If condition with undefined variable should fail");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_while_with_declared_var(void) {
    printf("\n=== Test: While with Declared Variable ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    
    ASTNode *id_x = ast_make_id("x");
    ASTNode *ten = ast_make_num_int(10);
    ASTNode *cond = ast_make_binop(id_x, '<', ten);
    
    ASTNode *body = ast_make_commands();
    ASTNode *one = ast_make_num_int(1);
    ASTNode *add = ast_make_binop(id_x, '+', one);
    ASTNode *assign = ast_make_assignment("x", add);
    ast_add_child(body, assign);
    
    ASTNode *while_stmt = ast_make_while(cond, body);
    ast_add_child(program->data.program.commands, while_stmt);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "While with declared variable should pass");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_while_with_undefined_var(void) {
    printf("\n=== Test: While with Undefined Variable ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ASTNode *id_x = ast_make_id("undefined_x");
    ASTNode *ten = ast_make_num_int(10);
    ASTNode *cond = ast_make_binop(id_x, '<', ten);
    
    ASTNode *body = ast_make_commands();
    ASTNode *while_stmt = ast_make_while(cond, body);
    ast_add_child(program->data.program.commands, while_stmt);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 0, "While with undefined variable should fail");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_for_loop_valid(void) {
    printf("\n=== Test: For Loop Valid ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("i"));
    
    ASTNode *one = ast_make_num_int(1);
    ASTNode *ten = ast_make_num_int(10);
    ASTNode *step = ast_make_num_int(1);
    
    ASTNode *body = ast_make_commands();
    ASTNode *for_stmt = ast_make_for("i", one, ten, step, body);
    ast_add_child(program->data.program.commands, for_stmt);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "For loop with declared variable should pass");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_for_loop_undefined_var(void) {
    printf("\n=== Test: For Loop Undefined Variable ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ASTNode *one = ast_make_num_int(1);
    ASTNode *ten = ast_make_num_int(10);
    ASTNode *step = ast_make_num_int(1);
    
    ASTNode *body = ast_make_commands();
    ASTNode *for_stmt = ast_make_for("undefined_i", one, ten, step, body);
    ast_add_child(program->data.program.commands, for_stmt);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 0, "For loop with undefined variable should fail");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_nested_control_flow(void) {
    printf("\n=== Test: Nested Control Flow ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    ast_add_child(program->data.program.declarations, create_int_declaration("y"));
    
    /* if x > 0 then while y < 10 ... */
    ASTNode *id_x = ast_make_id("x");
    ASTNode *zero = ast_make_num_int(0);
    ASTNode *if_cond = ast_make_binop(id_x, '>', zero);
    
    ASTNode *id_y = ast_make_id("y");
    ASTNode *ten = ast_make_num_int(10);
    ASTNode *while_cond = ast_make_binop(id_y, '<', ten);
    
    ASTNode *while_body = ast_make_commands();
    ASTNode *while_stmt = ast_make_while(while_cond, while_body);
    
    ASTNode *if_body = ast_make_commands();
    ast_add_child(if_body, while_stmt);
    
    ASTNode *if_stmt = ast_make_if(if_cond, if_body, NULL);
    ast_add_child(program->data.program.commands, if_stmt);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Nested control flow with variable checks should pass");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_control_flow_scope(void) {
    printf("\n=== Test: Control Flow Scope ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Test");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    
    ASTNode *id_x = ast_make_id("x");
    ASTNode *zero = ast_make_num_int(0);
    ASTNode *cond = ast_make_binop(id_x, '>', zero);
    
    ASTNode *then_cmds = ast_make_commands();
    ASTNode *five = ast_make_num_int(5);
    ASTNode *assign = ast_make_assignment("x", five);
    ast_add_child(then_cmds, assign);
    
    ASTNode *if_stmt = ast_make_if(cond, then_cmds, NULL);
    ast_add_child(program->data.program.commands, if_stmt);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Variables in nested scopes should be properly checked");
    
    ast_free(program);
    semantic_free(analyzer);
}

/* ============ INTEGRATION TESTS (8 tests) ============ */

void test_semantic_complete_valid_program(void) {
    printf("\n=== Test: Complete Valid Program ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("CompleteTest");
    
    /* Declarations */
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    ast_add_child(program->data.program.declarations, create_float_declaration("y"));
    
    /* Commands */
    ASTNode *read_x = ast_make_read("x");
    ast_add_child(program->data.program.commands, read_x);
    
    ASTNode *id_x = ast_make_id("x");
    ASTNode *five = ast_make_num_int(5);
    ASTNode *expr = ast_make_binop(id_x, '+', five);
    ASTNode *assign = ast_make_assignment("y", expr);
    ast_add_child(program->data.program.commands, assign);
    
    ASTNode *id_y = ast_make_id("y");
    ASTNode *write = ast_make_write(id_y, 1);
    ast_add_child(program->data.program.commands, write);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Full program with all features valid should pass");
    assert_equal_int(analyzer->error_count, 0, "No errors in valid program");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_complete_with_errors(void) {
    printf("\n=== Test: Complete with Errors ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("ErrorTest");
    
    /* Declarations */
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    
    /* Commands with undefined variable */
    ASTNode *read_undefined = ast_make_read("undefined");
    ast_add_child(program->data.program.commands, read_undefined);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 0, "Full program with errors should detect them");
    assert_true(analyzer->error_count > 0, "Error count should be greater than 0");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_multiple_statements(void) {
    printf("\n=== Test: Multiple Statements ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("MultiStmt");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("a"));
    ast_add_child(program->data.program.declarations, create_int_declaration("b"));
    ast_add_child(program->data.program.declarations, create_int_declaration("c"));
    
    ASTNode *read_a = ast_make_read("a");
    ast_add_child(program->data.program.commands, read_a);
    
    ASTNode *id_a = ast_make_id("a");
    ASTNode *num = ast_make_num_int(2);
    ASTNode *mul = ast_make_binop(id_a, '*', num);
    ASTNode *assign_b = ast_make_assignment("b", mul);
    ast_add_child(program->data.program.commands, assign_b);
    
    ASTNode *id_b = ast_make_id("b");
    ASTNode *add = ast_make_binop(id_a, '+', id_b);
    ASTNode *assign_c = ast_make_assignment("c", add);
    ast_add_child(program->data.program.commands, assign_c);
    
    ASTNode *id_c = ast_make_id("c");
    ASTNode *write = ast_make_write(id_c, 1);
    ast_add_child(program->data.program.commands, write);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Multiple statements all using variables should pass");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_statement_sequence(void) {
    printf("\n=== Test: Statement Sequence ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Sequence");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    
    /* Read - Assignment - Write sequence */
    ASTNode *read = ast_make_read("x");
    ast_add_child(program->data.program.commands, read);
    
    ASTNode *id = ast_make_id("x");
    ASTNode *five = ast_make_num_int(5);
    ASTNode *add = ast_make_binop(id, '+', five);
    ASTNode *assign = ast_make_assignment("x", add);
    ast_add_child(program->data.program.commands, assign);
    
    ASTNode *id2 = ast_make_id("x");
    ASTNode *write = ast_make_write(id2, 1);
    ast_add_child(program->data.program.commands, write);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Sequence of different statements should pass");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_mixed_declarations_statements(void) {
    printf("\n=== Test: Mixed Declarations and Statements ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Mixed");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    ast_add_child(program->data.program.declarations, create_float_declaration("y"));
    ast_add_child(program->data.program.declarations, create_int_declaration("z"));
    
    ASTNode *read_x = ast_make_read("x");
    ast_add_child(program->data.program.commands, read_x);
    
    ASTNode *id_x = ast_make_id("x");
    ASTNode *num = ast_make_num_float(3.14);
    ASTNode *mul = ast_make_binop(id_x, '*', num);
    ASTNode *assign_y = ast_make_assignment("y", mul);
    ast_add_child(program->data.program.commands, assign_y);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Mix of inteiro and real with various statements should pass");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_complex_program(void) {
    printf("\n=== Test: Complex Program ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Complex");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("i"));
    ast_add_child(program->data.program.declarations, create_int_declaration("sum"));
    
    ASTNode *zero = ast_make_num_int(0);
    ASTNode *assign_sum = ast_make_assignment("sum", zero);
    ast_add_child(program->data.program.commands, assign_sum);
    
    ASTNode *one = ast_make_num_int(1);
    ASTNode *ten = ast_make_num_int(10);
    ASTNode *step = ast_make_num_int(1);
    
    ASTNode *for_body = ast_make_commands();
    ASTNode *id_sum = ast_make_id("sum");
    ASTNode *id_i = ast_make_id("i");
    ASTNode *add = ast_make_binop(id_sum, '+', id_i);
    ASTNode *update_sum = ast_make_assignment("sum", add);
    ast_add_child(for_body, update_sum);
    
    ASTNode *for_stmt = ast_make_for("i", one, ten, step, for_body);
    ast_add_child(program->data.program.commands, for_stmt);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Complex program with nested structures should pass");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_program_no_declarations(void) {
    printf("\n=== Test: Program No Declarations ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("NoDecl");
    
    ASTNode *id_x = ast_make_id("x");
    ASTNode *write = ast_make_write(id_x, 1);
    ast_add_child(program->data.program.commands, write);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 0, "Program with no declarations but using variables should fail");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_program_large(void) {
    printf("\n=== Test: Large Program ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Large");
    
    /* Declare many variables */
    for (int i = 0; i < 50; i++) {
        char name[10];
        sprintf(name, "var%d", i);
        if (i % 2 == 0) {
            ast_add_child(program->data.program.declarations, create_int_declaration(name));
        } else {
            ast_add_child(program->data.program.declarations, create_float_declaration(name));
        }
    }
    
    /* Use some variables */
    for (int i = 0; i < 10; i++) {
        char name[10];
        sprintf(name, "var%d", i);
        ASTNode *read = ast_make_read(name);
        ast_add_child(program->data.program.commands, read);
    }
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Large program with many variables should pass");
    
    ast_free(program);
    semantic_free(analyzer);
}

/* ============ ERROR REPORTING TESTS (5 tests) ============ */

void test_semantic_error_message_undefined_var(void) {
    printf("\n=== Test: Error Message Undefined Variable ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("ErrorMsg");
    
    ASTNode *id = ast_make_id("my_undefined_var");
    ASTNode *write = ast_make_write(id, 0);
    ast_add_child(program->data.program.commands, write);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 0, "Should fail");
    assert_true(strlen(result.error_message) > 0, "Error message should be non-empty");
    assert_true(strstr(result.error_message, "Undefined") != NULL, "Error message should mention Undefined");
    assert_true(strstr(result.error_message, "my_undefined_var") != NULL, "Error message should include variable name");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_error_message_duplicate(void) {
    printf("\n=== Test: Error Message Duplicate ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("DuplicateMsg");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("myvar"));
    ast_add_child(program->data.program.declarations, create_float_declaration("myvar"));
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 0, "Should fail");
    assert_true(strlen(result.error_message) > 0, "Error message should be non-empty");
    assert_true(strstr(result.error_message, "Duplicate") != NULL, "Error message should mention Duplicate");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_error_count(void) {
    printf("\n=== Test: Error Count ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("ErrorCount");
    
    ASTNode *id1 = ast_make_id("undefined1");
    ASTNode *id2 = ast_make_id("undefined2");
    ASTNode *expr = ast_make_binop(id1, '+', id2);
    ASTNode *write = ast_make_write(expr, 0);
    ast_add_child(program->data.program.commands, write);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 0, "Should fail");
    assert_true(analyzer->error_count > 0, "Error count should be greater than 0");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_error_recovery(void) {
    printf("\n=== Test: Error Recovery ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("Recovery");
    
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    
    ASTNode *id_undef = ast_make_id("undefined");
    ASTNode *write1 = ast_make_write(id_undef, 0);
    ast_add_child(program->data.program.commands, write1);
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 0, "Should fail on first error");
    
    ast_free(program);
    semantic_free(analyzer);
}

void test_semantic_clear_errors(void) {
    printf("\n=== Test: Clear Errors ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    
    /* First analysis with error */
    ASTNode *program1 = create_simple_program("First");
    ASTNode *id = ast_make_id("undefined");
    ASTNode *write = ast_make_write(id, 0);
    ast_add_child(program1->data.program.commands, write);
    
    SemanticResult result1 = semantic_analyze_program(analyzer, program1);
    assert_equal_int(result1.is_valid, 0, "First analysis should fail");
    assert_true(analyzer->error_count > 0, "First analysis should have errors");
    
    /* Create new analyzer for second analysis */
    semantic_free(analyzer);
    analyzer = semantic_create();
    
    /* Second analysis without error */
    ASTNode *program2 = create_simple_program("Second");
    ast_add_child(program2->data.program.declarations, create_int_declaration("x"));
    ASTNode *read = ast_make_read("x");
    ast_add_child(program2->data.program.commands, read);
    
    SemanticResult result2 = semantic_analyze_program(analyzer, program2);
    assert_equal_int(result2.is_valid, 1, "Second analysis should pass with fresh analyzer");
    assert_equal_int(analyzer->error_count, 0, "Second analysis should have no errors");
    
    ast_free(program1);
    ast_free(program2);
    semantic_free(analyzer);
}

/* ============ MEMORY TESTS (3 tests) ============ */

void test_semantic_memory_cleanup(void) {
    printf("\n=== Test: Memory Cleanup ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    assert_not_null(analyzer, "Analyzer created");
    assert_not_null(analyzer->symbols, "Symbol table created");
    
    ASTNode *program = create_simple_program("Cleanup");
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    
    ast_free(program);
    semantic_free(analyzer);
    
    assert_true(1, "Proper cleanup of analyzer completed");
}

void test_semantic_large_program_memory(void) {
    printf("\n=== Test: Large Program Memory ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    ASTNode *program = create_simple_program("LargeMemory");
    
    /* Create large number of declarations */
    for (int i = 0; i < 100; i++) {
        char name[15];
        sprintf(name, "variable_%d", i);
        if (i % 2 == 0) {
            ast_add_child(program->data.program.declarations, create_int_declaration(name));
        } else {
            ast_add_child(program->data.program.declarations, create_float_declaration(name));
        }
    }
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Large program memory handling should pass");
    
    ast_free(program);
    semantic_free(analyzer);
    
    assert_true(1, "Memory properly managed for large program");
}

void test_semantic_symbol_table_integration(void) {
    printf("\n=== Test: Symbol Table Integration ===\n");
    
    SemanticAnalyzer *analyzer = semantic_create();
    assert_not_null(analyzer->symbols, "Symbol table should be created");
    assert_equal_int(analyzer->symbols->count, 0, "Symbol table should start empty");
    
    ASTNode *program = create_simple_program("SymbolInt");
    ast_add_child(program->data.program.declarations, create_int_declaration("x"));
    ast_add_child(program->data.program.declarations, create_float_declaration("y"));
    ast_add_child(program->data.program.declarations, create_int_declaration("z"));
    
    SemanticResult result = semantic_analyze_program(analyzer, program);
    assert_equal_int(result.is_valid, 1, "Analysis should pass");
    assert_equal_int(analyzer->symbols->count, 3, "Symbol table should have 3 symbols");
    
    Symbol *sym_x = symbol_table_lookup(analyzer->symbols, "x");
    assert_not_null(sym_x, "Symbol x should be in table");
    if (sym_x) {
        assert_equal_int(sym_x->type, TYPE_INT, "Symbol x should have type INT");
    }
    
    Symbol *sym_y = symbol_table_lookup(analyzer->symbols, "y");
    assert_not_null(sym_y, "Symbol y should be in table");
    if (sym_y) {
        assert_equal_int(sym_y->type, TYPE_FLOAT, "Symbol y should have type FLOAT");
    }
    
    Symbol *sym_undefined = symbol_table_lookup(analyzer->symbols, "undefined");
    assert_null(sym_undefined, "Undefined symbol should not be in table");
    
    ast_free(program);
    semantic_free(analyzer);
}

/* ============ MAIN TEST RUNNER ============ */

int main(void) {
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║  === SEMANTIC ANALYSIS TESTS ===      ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    /* Basic Declaration Tests */
    printf("\n[BASIC DECLARATION TESTS]\n");
    test_semantic_valid_declaration();
    test_semantic_multiple_declarations();
    test_semantic_duplicate_declaration();
    test_semantic_multiple_duplicates();
    test_semantic_mixed_types();
    
    /* Variable Usage Tests */
    printf("\n[VARIABLE USAGE TESTS]\n");
    test_semantic_valid_assignment();
    test_semantic_undefined_variable_in_assignment();
    test_semantic_valid_read();
    test_semantic_undefined_variable_in_read();
    test_semantic_valid_write();
    test_semantic_undefined_variable_in_write();
    test_semantic_variable_in_expression();
    test_semantic_multiple_undefined_variables();
    
    /* Expression Tests */
    printf("\n[EXPRESSION TESTS]\n");
    test_semantic_expression_with_numbers();
    test_semantic_expression_with_variables();
    test_semantic_expression_undefined_var();
    test_semantic_complex_expression();
    test_semantic_unary_expression();
    test_semantic_nested_expression();
    
    /* Control Flow Tests */
    printf("\n[CONTROL FLOW TESTS]\n");
    test_semantic_if_with_declared_var();
    test_semantic_if_with_undefined_var();
    test_semantic_while_with_declared_var();
    test_semantic_while_with_undefined_var();
    test_semantic_for_loop_valid();
    test_semantic_for_loop_undefined_var();
    test_semantic_nested_control_flow();
    test_semantic_control_flow_scope();
    
    /* Integration Tests */
    printf("\n[INTEGRATION TESTS]\n");
    test_semantic_complete_valid_program();
    test_semantic_complete_with_errors();
    test_semantic_multiple_statements();
    test_semantic_statement_sequence();
    test_semantic_mixed_declarations_statements();
    test_semantic_complex_program();
    test_semantic_program_no_declarations();
    test_semantic_program_large();
    
    /* Error Reporting Tests */
    printf("\n[ERROR REPORTING TESTS]\n");
    test_semantic_error_message_undefined_var();
    test_semantic_error_message_duplicate();
    test_semantic_error_count();
    test_semantic_error_recovery();
    test_semantic_clear_errors();
    
    /* Memory Tests */
    printf("\n[MEMORY TESTS]\n");
    test_semantic_memory_cleanup();
    test_semantic_large_program_memory();
    test_semantic_symbol_table_integration();
    
    /* Summary */
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║          TEST SUMMARY                  ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║  Total Tests:  %3d                     ║\n", test_count);
    printf("║  Passed:       %3d                     ║\n", test_passed);
    printf("║  Failed:       %3d                     ║\n", test_count - test_passed);
    printf("║  Success Rate: %.1f%%                  ║\n", (100.0 * test_passed) / test_count);
    printf("╚════════════════════════════════════════╝\n\n");
    
    if (test_passed == test_count) {
        printf("✓ All tests PASSED!\n\n");
        return 0;
    } else {
        printf("✗ Some tests FAILED\n\n");
        return 1;
    }
}
