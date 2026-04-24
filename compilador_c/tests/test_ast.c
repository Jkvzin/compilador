#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/ast.h"

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

void test_create_simple_program() {
    printf("\n=== Test: Create Simple Program ===\n");
    
    ASTNode *decls = ast_make_declarations();
    assert_true(decls != NULL, "Declarations list created");
    assert_true(decls->type == NODE_DECLARATIONS, "Declarations type is correct");
    assert_true(decls->data.declarations.count == 0, "Declarations count is 0");
    
    ASTNode *cmds = ast_make_commands();
    assert_true(cmds != NULL, "Commands list created");
    assert_true(cmds->type == NODE_COMMANDS, "Commands type is correct");
    assert_true(cmds->data.commands.count == 0, "Commands count is 0");
    
    ASTNode *prog = ast_make_program("Teste", decls, cmds);
    assert_true(prog != NULL, "Program node created");
    assert_true(prog->type == NODE_PROGRAM, "Program type is correct");
    assert_true(strcmp(prog->data.program.name, "Teste") == 0, "Program name is correct");
    assert_true(prog->data.program.declarations == decls, "Program declarations pointer is correct");
    assert_true(prog->data.program.commands == cmds, "Program commands pointer is correct");
    
    printf("\nPrinting program structure:\n");
    ast_print(prog, 0);
    
    ast_free(prog);
    printf("\nProgram freed successfully\n");
}

void test_add_declarations() {
    printf("\n=== Test: Add Declarations ===\n");
    
    ASTNode *decls = ast_make_declarations();
    assert_true(decls != NULL, "Declarations list created");
    
    ASTNode *decl1 = ast_make_declaration(TYPE_INT, "x");
    assert_true(decl1 != NULL, "Declaration 1 created");
    assert_true(decl1->data.declaration.type == TYPE_INT, "Declaration 1 type is INT");
    
    ASTNode *decl2 = ast_make_declaration(TYPE_FLOAT, "y");
    assert_true(decl2 != NULL, "Declaration 2 created");
    assert_true(decl2->data.declaration.type == TYPE_FLOAT, "Declaration 2 type is FLOAT");
    
    ast_add_child(decls, decl1);
    assert_true(decls->data.declarations.count == 1, "Declaration count after adding 1");
    
    ast_add_child(decls, decl2);
    assert_true(decls->data.declarations.count == 2, "Declaration count after adding 2");
    
    printf("\nPrinting declarations:\n");
    ast_print(decls, 0);
    
    ast_free(decls);
    printf("\nDeclarations freed successfully\n");
}

void test_add_commands() {
    printf("\n=== Test: Add Commands ===\n");
    
    ASTNode *cmds = ast_make_commands();
    assert_true(cmds != NULL, "Commands list created");
    
    /* Create assignment: x <- 5 */
    ASTNode *five = ast_make_num_int(5);
    ASTNode *assign = ast_make_assignment("x", five);
    assert_true(assign != NULL, "Assignment created");
    assert_true(assign->type == NODE_ASSIGNMENT, "Assignment type is correct");
    
    /* Create read: leia y */
    ASTNode *read = ast_make_read("y");
    assert_true(read != NULL, "Read created");
    assert_true(read->type == NODE_READ, "Read type is correct");
    
    ast_add_child(cmds, assign);
    assert_true(cmds->data.commands.count == 1, "Command count after adding assignment");
    
    ast_add_child(cmds, read);
    assert_true(cmds->data.commands.count == 2, "Command count after adding read");
    
    printf("\nPrinting commands:\n");
    ast_print(cmds, 0);
    
    ast_free(cmds);
    printf("\nCommands freed successfully\n");
}

void test_expressions() {
    printf("\n=== Test: Expressions ===\n");
    
    /* Create: x + 10 */
    ASTNode *id_x = ast_make_id("x");
    ASTNode *ten = ast_make_num_int(10);
    ASTNode *add = ast_make_binop(id_x, '+', ten);
    
    assert_true(id_x != NULL, "ID node created");
    assert_true(ten != NULL, "Integer node created");
    assert_true(add != NULL, "Binary operation created");
    assert_true(add->data.binop.op == '+', "Binary op is +");
    
    printf("\nPrinting expression (x + 10):\n");
    ast_print(add, 0);
    
    ast_free(add);
    printf("\nExpression freed successfully\n");
}

void test_control_flow() {
    printf("\n=== Test: Control Flow ===\n");
    
    /* Create: se x > 0 entao ... */
    ASTNode *x = ast_make_id("x");
    ASTNode *zero = ast_make_num_int(0);
    ASTNode *cond = ast_make_binop(x, '>', zero);
    
    ASTNode *then_cmds = ast_make_commands();
    ASTNode *five = ast_make_num_int(5);
    ASTNode *assign = ast_make_assignment("result", five);
    ast_add_child(then_cmds, assign);
    
    ASTNode *if_node = ast_make_if(cond, then_cmds, NULL);
    
    assert_true(if_node != NULL, "IF node created");
    assert_true(if_node->type == NODE_IF, "IF type is correct");
    assert_true(if_node->data.if_stmt.condition != NULL, "IF has condition");
    assert_true(if_node->data.if_stmt.then_branch != NULL, "IF has then branch");
    
    printf("\nPrinting if statement:\n");
    ast_print(if_node, 0);
    
    ast_free(if_node);
    printf("\nIF statement freed successfully\n");
}

void test_loops() {
    printf("\n=== Test: Loops ===\n");
    
    /* Create while loop */
    ASTNode *x1 = ast_make_id("x");
    ASTNode *ten = ast_make_num_int(10);
    ASTNode *cond = ast_make_binop(x1, '<', ten);
    
    ASTNode *body = ast_make_commands();
    ASTNode *one = ast_make_num_int(1);
    ASTNode *x2 = ast_make_id("x");
    ASTNode *add = ast_make_binop(x2, '+', one);
    ASTNode *assign = ast_make_assignment("x", add);
    ast_add_child(body, assign);
    
    ASTNode *while_node = ast_make_while(cond, body);
    
    assert_true(while_node != NULL, "WHILE node created");
    assert_true(while_node->type == NODE_WHILE, "WHILE type is correct");
    
    printf("\nPrinting while loop:\n");
    ast_print(while_node, 0);
    
    ast_free(while_node);
    printf("\nWHILE loop freed successfully\n");
}

void test_numeric_types() {
    printf("\n=== Test: Numeric Types ===\n");
    
    /* Test integer */
    ASTNode *int_node = ast_make_num_int(42);
    assert_true(int_node != NULL, "Integer node created");
    assert_true(int_node->type == NODE_NUM_INT, "Integer type is correct");
    assert_true(int_node->data.num_int.value == 42, "Integer value is correct");
    ast_free(int_node);
    
    /* Test float */
    ASTNode *float_node = ast_make_num_float(3.14);
    assert_true(float_node != NULL, "Float node created");
    assert_true(float_node->type == NODE_NUM_FLOAT, "Float type is correct");
    assert_true(float_node->data.num_float.value == 3.14, "Float value is correct");
    ast_free(float_node);
    
    printf("\nNumeric types test passed\n");
}

int main(void) {
    printf("====== AST Node Tests ======\n");
    
    test_create_simple_program();
    test_add_declarations();
    test_add_commands();
    test_expressions();
    test_control_flow();
    test_loops();
    test_numeric_types();
    
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
