#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/parser.h"
#include "../src/lexer.h"

/* Test counter */
static int test_count = 0;
static int test_passed = 0;
static int assertions = 0;
static int assertions_passed = 0;

#define TEST_START(name) \
    do { \
        test_count++; \
        printf("\n--- Test: %s ---\n", name); \
    } while (0)

#define ASSERT(condition) \
    do { \
        assertions++; \
        if (condition) { \
            assertions_passed++; \
            printf("  ✓ Assertion passed\n"); \
        } else { \
            printf("  ✗ Assertion failed: %s\n", #condition); \
        } \
    } while (0)

#define ASSERT_EQ(a, b) \
    do { \
        assertions++; \
        if ((a) == (b)) { \
            assertions_passed++; \
            printf("  ✓ %s == %s\n", #a, #b); \
        } else { \
            printf("  ✗ %s != %s\n", #a, #b); \
        } \
    } while (0)

#define ASSERT_STR_EQ(a, b) \
    do { \
        assertions++; \
        if (strcmp((a), (b)) == 0) { \
            assertions_passed++; \
            printf("  ✓ \"%s\" == \"%s\"\n", (a), (b)); \
        } else { \
            printf("  ✗ \"%s\" != \"%s\"\n", (a), (b)); \
        } \
    } while (0)

#define TEST_END() \
    do { \
        test_passed++; \
        printf("  [PASSED]\n"); \
    } while (0)

/* Test: Create and free parser */
void test_create_parser(void) {
    TEST_START("test_create_parser");
    
    TokenList tokens = {0};
    tokens.tokens = malloc(sizeof(Token));
    tokens.capacity = 1;
    tokens.count = 0;
    
    Parser *parser = parser_create(&tokens);
    ASSERT(parser != NULL);
    ASSERT_EQ(parser->current, 0);
    ASSERT(parser->tokens == &tokens);
    
    parser_free(parser);
    free(tokens.tokens);
    
    TEST_END();
}

/* Test: Parse empty program skeleton */
void test_parse_empty_program(void) {
    TEST_START("test_parse_empty_program");
    
    const char *program = "programa TESTE\ninicio\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASSERT(parser != NULL);
    
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ASSERT_EQ(ast->type, NODE_PROGRAM);
    ASSERT_STR_EQ(ast->data.program.name, "TESTE");
    ASSERT(ast->data.program.declarations != NULL);
    ASSERT(ast->data.program.commands != NULL);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse single integer declaration */
void test_parse_simple_declaration(void) {
    TEST_START("test_parse_simple_declaration");
    
    const char *program = "programa TEST\ninteiro x;\ninicio\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASSERT(parser != NULL);
    
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ASSERT_EQ(ast->type, NODE_PROGRAM);
    
    ASTNode *decls = ast->data.program.declarations;
    ASSERT(decls != NULL);
    ASSERT_EQ(decls->type, NODE_DECLARATIONS);
    ASSERT_EQ(decls->data.declarations.count, 1);
    
    ASTNode *decl = decls->data.declarations.items[0];
    ASSERT_EQ(decl->type, NODE_DECLARATION);
    ASSERT_EQ(decl->data.declaration.type, TYPE_INT);
    ASSERT_STR_EQ(decl->data.declaration.name, "x");
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse multiple declarations */
void test_parse_multiple_declarations(void) {
    TEST_START("test_parse_multiple_declarations");
    
    const char *program = "programa PROG\ninteiro x;\ninteiro y;\nflutuante z;\ninicio\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASSERT(parser != NULL);
    
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ASSERT_EQ(ast->type, NODE_PROGRAM);
    
    ASTNode *decls = ast->data.program.declarations;
    ASSERT(decls != NULL);
    ASSERT_EQ(decls->data.declarations.count, 3);
    
    /* Check first declaration (inteiro x) */
    ASTNode *decl1 = decls->data.declarations.items[0];
    ASSERT_EQ(decl1->type, NODE_DECLARATION);
    ASSERT_EQ(decl1->data.declaration.type, TYPE_INT);
    ASSERT_STR_EQ(decl1->data.declaration.name, "x");
    
    /* Check second declaration (inteiro y) */
    ASTNode *decl2 = decls->data.declarations.items[1];
    ASSERT_EQ(decl2->type, NODE_DECLARATION);
    ASSERT_EQ(decl2->data.declaration.type, TYPE_INT);
    ASSERT_STR_EQ(decl2->data.declaration.name, "y");
    
    /* Check third declaration (flutuante z) */
    ASTNode *decl3 = decls->data.declarations.items[2];
    ASSERT_EQ(decl3->type, NODE_DECLARATION);
    ASSERT_EQ(decl3->data.declaration.type, TYPE_FLOAT);
    ASSERT_STR_EQ(decl3->data.declaration.name, "z");
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse program structure */
void test_parse_program_structure(void) {
    TEST_START("test_parse_program_structure");
    
    const char *program = "programa MYAPP\ninteiro a;\nflutuante b;\ninicio\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASSERT(parser != NULL);
    
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    /* Verify program structure */
    ASSERT_EQ(ast->type, NODE_PROGRAM);
    ASSERT_STR_EQ(ast->data.program.name, "MYAPP");
    
    ASSERT(ast->data.program.declarations != NULL);
    ASSERT_EQ(ast->data.program.declarations->type, NODE_DECLARATIONS);
    
    ASSERT(ast->data.program.commands != NULL);
    ASSERT_EQ(ast->data.program.commands->type, NODE_COMMANDS);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parser helper functions */
void test_parser_helpers(void) {
    TEST_START("test_parser_helpers");
    
    const char *program = "programa X\ninicio\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASSERT(parser != NULL);
    
    /* Test check function */
    Token *token = parser_current_token(parser);
    ASSERT(token != NULL);
    ASSERT(parser_check(parser, TOK_PROGRAMA));
    ASSERT(!parser_check(parser, TOK_INICIO));
    
    /* Test advance */
    parser_advance(parser);
    token = parser_current_token(parser);
    ASSERT(parser_check(parser, TOK_ID));
    
    /* Test match */
    ASSERT(parser_match(parser, TOK_ID));
    token = parser_current_token(parser);
    ASSERT(parser_check(parser, TOK_INICIO));
    
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Error handling */
void test_error_handling(void) {
    TEST_START("test_error_handling");
    
    /* Test: Missing 'programa' keyword */
    const char *bad_program1 = "inteiro x;\ninicio\nfim.";
    TokenList *tokens1 = lexer_tokenize(bad_program1);
    ASSERT(tokens1 != NULL);
    
    Parser *parser1 = parser_create(tokens1);
    ASTNode *ast1 = parser_parse_program(parser1);
    ASSERT(ast1 == NULL);
    
    parser_free(parser1);
    token_list_free(tokens1);
    
    /* Test: Missing 'inicio' keyword */
    const char *bad_program2 = "programa TEST\ninteiro x;\nfim.";
    TokenList *tokens2 = lexer_tokenize(bad_program2);
    ASSERT(tokens2 != NULL);
    
    Parser *parser2 = parser_create(tokens2);
    ASTNode *ast2 = parser_parse_program(parser2);
    ASSERT(ast2 == NULL);
    
    parser_free(parser2);
    token_list_free(tokens2);
    
    /* Test: Missing 'fim' keyword */
    const char *bad_program3 = "programa TEST\ninteiro x;\ninicio";
    TokenList *tokens3 = lexer_tokenize(bad_program3);
    ASSERT(tokens3 != NULL);
    
    Parser *parser3 = parser_create(tokens3);
    ASTNode *ast3 = parser_parse_program(parser3);
    ASSERT(ast3 == NULL);
    
    parser_free(parser3);
    token_list_free(tokens3);
    
    TEST_END();
}

/* Test: Declarations without commands */
void test_declarations_only(void) {
    TEST_START("test_declarations_only");
    
    const char *program = "programa VARS\ninteiro a;\ninteiro b;\ninteiro c;\ninicio\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *decls = ast->data.program.declarations;
    ASSERT_EQ(decls->data.declarations.count, 3);
    
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 0);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Mixed types */
void test_mixed_types(void) {
    TEST_START("test_mixed_types");
    
    const char *program = "programa MIXED\ninteiro x;\nflutuante y;\ninteiro z;\ninicio\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *decls = ast->data.program.declarations;
    ASSERT_EQ(decls->data.declarations.count, 3);
    
    ASSERT_EQ(decls->data.declarations.items[0]->data.declaration.type, TYPE_INT);
    ASSERT_EQ(decls->data.declarations.items[1]->data.declaration.type, TYPE_FLOAT);
    ASSERT_EQ(decls->data.declarations.items[2]->data.declaration.type, TYPE_INT);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Complex program */
void test_complex_program(void) {
    TEST_START("test_complex_program");
    
    const char *program = 
        "programa COMPLETO\n"
        "inteiro x;\n"
        "inteiro y;\n"
        "flutuante total;\n"
        "flutuante media;\n"
        "inicio\n"
        "fim.";
    
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ASSERT_EQ(ast->type, NODE_PROGRAM);
    
    ASTNode *decls = ast->data.program.declarations;
    ASSERT_EQ(decls->data.declarations.count, 4);
    
    ASSERT_STR_EQ(decls->data.declarations.items[0]->data.declaration.name, "x");
    ASSERT_STR_EQ(decls->data.declarations.items[1]->data.declaration.name, "y");
    ASSERT_STR_EQ(decls->data.declarations.items[2]->data.declaration.name, "total");
    ASSERT_STR_EQ(decls->data.declarations.items[3]->data.declaration.name, "media");
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse simple number */
void test_parse_simple_number(void) {
    TEST_START("test_parse_simple_number");
    
    const char *program = "programa TEST\ninteiro x;\ninicio\nx <- 42;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ASSERT_EQ(ast->type, NODE_PROGRAM);
    
    /* Check assignment in commands */
    ASTNode *cmds = ast->data.program.commands;
    ASSERT(cmds != NULL);
    ASSERT_EQ(cmds->data.commands.count, 1);
    
    ASTNode *assign = cmds->data.commands.items[0];
    ASSERT_EQ(assign->type, NODE_ASSIGNMENT);
    ASSERT_STR_EQ(assign->data.assignment.var_name, "x");
    
    /* Check expression is a number */
    ASTNode *expr = assign->data.assignment.expr;
    ASSERT(expr != NULL);
    ASSERT_EQ(expr->type, NODE_NUM_INT);
    ASSERT_EQ(expr->data.num_int.value, 42);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse identifier */
void test_parse_identifier(void) {
    TEST_START("test_parse_identifier");
    
    const char *program = "programa TEST\ninteiro x;\ninteiro y;\ninicio\ny <- x;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 1);
    
    ASTNode *assign = cmds->data.commands.items[0];
    ASSERT_EQ(assign->type, NODE_ASSIGNMENT);
    
    ASTNode *expr = assign->data.assignment.expr;
    ASSERT_EQ(expr->type, NODE_ID);
    ASSERT_STR_EQ(expr->data.id.name, "x");
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse binary operator */
void test_parse_binary_operator(void) {
    TEST_START("test_parse_binary_operator");
    
    const char *program = "programa TEST\ninteiro x;\ninteiro y;\ninteiro z;\ninicio\nz <- 2 + 3;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 1);
    
    ASTNode *assign = cmds->data.commands.items[0];
    ASTNode *expr = assign->data.assignment.expr;
    ASSERT_EQ(expr->type, NODE_BINOP);
    ASSERT_EQ(expr->data.binop.op, '+');
    
    /* Check left operand */
    ASTNode *left = expr->data.binop.left;
    ASSERT_EQ(left->type, NODE_NUM_INT);
    ASSERT_EQ(left->data.num_int.value, 2);
    
    /* Check right operand */
    ASTNode *right = expr->data.binop.right;
    ASSERT_EQ(right->type, NODE_NUM_INT);
    ASSERT_EQ(right->data.num_int.value, 3);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse operator precedence */
void test_parse_operator_precedence(void) {
    TEST_START("test_parse_operator_precedence");
    
    /* 2 + 3 * 4 should parse as 2 + (3 * 4), not (2 + 3) * 4 */
    const char *program = "programa TEST\ninteiro x;\ninicio\nx <- 2 + 3 * 4;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASTNode *assign = cmds->data.commands.items[0];
    ASTNode *expr = assign->data.assignment.expr;
    
    /* Top level should be addition */
    ASSERT_EQ(expr->type, NODE_BINOP);
    ASSERT_EQ(expr->data.binop.op, '+');
    
    /* Left should be 2 */
    ASTNode *left = expr->data.binop.left;
    ASSERT_EQ(left->type, NODE_NUM_INT);
    ASSERT_EQ(left->data.num_int.value, 2);
    
    /* Right should be multiplication */
    ASTNode *right = expr->data.binop.right;
    ASSERT_EQ(right->type, NODE_BINOP);
    ASSERT_EQ(right->data.binop.op, '*');
    
    /* Right->left should be 3 */
    ASTNode *right_left = right->data.binop.left;
    ASSERT_EQ(right_left->type, NODE_NUM_INT);
    ASSERT_EQ(right_left->data.num_int.value, 3);
    
    /* Right->right should be 4 */
    ASTNode *right_right = right->data.binop.right;
    ASSERT_EQ(right_right->type, NODE_NUM_INT);
    ASSERT_EQ(right_right->data.num_int.value, 4);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse unary operator */
void test_parse_unary_operator(void) {
    TEST_START("test_parse_unary_operator");
    
    const char *program = "programa TEST\ninteiro x;\ninteiro y;\ninicio\nx <- -5;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASTNode *assign = cmds->data.commands.items[0];
    ASTNode *expr = assign->data.assignment.expr;
    
    ASSERT_EQ(expr->type, NODE_UNOP);
    ASSERT_EQ(expr->data.unop.op, '-');
    
    ASTNode *operand = expr->data.unop.operand;
    ASSERT_EQ(operand->type, NODE_NUM_INT);
    ASSERT_EQ(operand->data.num_int.value, 5);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse parenthesized expression */
void test_parse_parenthesized_expression(void) {
    TEST_START("test_parse_parenthesized_expression");
    
    /* (2 + 3) * 4 should have multiplication at top level */
    const char *program = "programa TEST\ninteiro x;\ninicio\nx <- (2 + 3) * 4;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASTNode *assign = cmds->data.commands.items[0];
    ASTNode *expr = assign->data.assignment.expr;
    
    /* Top level should be multiplication */
    ASSERT_EQ(expr->type, NODE_BINOP);
    ASSERT_EQ(expr->data.binop.op, '*');
    
    /* Left should be addition */
    ASTNode *left = expr->data.binop.left;
    ASSERT_EQ(left->type, NODE_BINOP);
    ASSERT_EQ(left->data.binop.op, '+');
    
    /* Right should be 4 */
    ASTNode *right = expr->data.binop.right;
    ASSERT_EQ(right->type, NODE_NUM_INT);
    ASSERT_EQ(right->data.num_int.value, 4);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse complex expression */
void test_parse_complex_expression(void) {
    TEST_START("test_parse_complex_expression");
    
    /* x > 5 + 2 should parse as x > (5 + 2) */
    const char *program = "programa TEST\ninteiro x;\ninteiro y;\ninicio\ny <- x > 5 + 2;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASTNode *assign = cmds->data.commands.items[0];
    ASTNode *expr = assign->data.assignment.expr;
    
    /* Top level should be comparison */
    ASSERT_EQ(expr->type, NODE_BINOP);
    ASSERT_EQ(expr->data.binop.op, '>');
    
    /* Left should be x */
    ASTNode *left = expr->data.binop.left;
    ASSERT_EQ(left->type, NODE_ID);
    ASSERT_STR_EQ(left->data.id.name, "x");
    
    /* Right should be addition */
    ASTNode *right = expr->data.binop.right;
    ASSERT_EQ(right->type, NODE_BINOP);
    ASSERT_EQ(right->data.binop.op, '+');
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Assignment statement */
void test_parse_assignment(void) {
    TEST_START("test_parse_assignment");
    
    const char *program = "programa TEST\ninteiro x;\ninteiro y;\ninteiro z;\ninicio\nx <- 10;\ny <- 20;\nz <- x + y;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 3);
    
    /* First assignment: x <- 10 */
    ASTNode *assign1 = cmds->data.commands.items[0];
    ASSERT_EQ(assign1->type, NODE_ASSIGNMENT);
    ASSERT_STR_EQ(assign1->data.assignment.var_name, "x");
    
    /* Second assignment: y <- 20 */
    ASTNode *assign2 = cmds->data.commands.items[1];
    ASSERT_EQ(assign2->type, NODE_ASSIGNMENT);
    ASSERT_STR_EQ(assign2->data.assignment.var_name, "y");
    
    /* Third assignment: z <- x + y */
    ASTNode *assign3 = cmds->data.commands.items[2];
    ASSERT_EQ(assign3->type, NODE_ASSIGNMENT);
    ASSERT_STR_EQ(assign3->data.assignment.var_name, "z");
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Left-associative operators */
void test_parse_left_associativity(void) {
    TEST_START("test_parse_left_associativity");
    
    /* 10 - 5 - 2 should parse as (10 - 5) - 2 = 3 */
    const char *program = "programa TEST\ninteiro x;\ninicio\nx <- 10 - 5 - 2;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASTNode *assign = cmds->data.commands.items[0];
    ASTNode *expr = assign->data.assignment.expr;
    
    /* Top level should be subtraction */
    ASSERT_EQ(expr->type, NODE_BINOP);
    ASSERT_EQ(expr->data.binop.op, '-');
    
    /* Left should be another subtraction (10 - 5) */
    ASTNode *left = expr->data.binop.left;
    ASSERT_EQ(left->type, NODE_BINOP);
    ASSERT_EQ(left->data.binop.op, '-');
    
    /* Left-left should be 10 */
    ASTNode *left_left = left->data.binop.left;
    ASSERT_EQ(left_left->type, NODE_NUM_INT);
    ASSERT_EQ(left_left->data.num_int.value, 10);
    
    /* Left-right should be 5 */
    ASTNode *left_right = left->data.binop.right;
    ASSERT_EQ(left_right->type, NODE_NUM_INT);
    ASSERT_EQ(left_right->data.num_int.value, 5);
    
    /* Right should be 2 */
    ASTNode *right = expr->data.binop.right;
    ASSERT_EQ(right->type, NODE_NUM_INT);
    ASSERT_EQ(right->data.num_int.value, 2);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Comparison operators */
void test_parse_comparison_operators(void) {
    TEST_START("test_parse_comparison_operators");
    
    const char *program = "programa TEST\ninteiro x;\ninteiro y;\ninicio\nx <- 5;\ny <- x = 5;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASTNode *assign2 = cmds->data.commands.items[1];
    ASTNode *expr = assign2->data.assignment.expr;
    
    ASSERT_EQ(expr->type, NODE_BINOP);
    ASSERT_EQ(expr->data.binop.op, '=');
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Float literals */
void test_parse_float_literal(void) {
    TEST_START("test_parse_float_literal");
    
    const char *program = "programa TEST\nflutuante x;\ninicio\nx <- 3.14;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASTNode *assign = cmds->data.commands.items[0];
    ASTNode *expr = assign->data.assignment.expr;
    
    ASSERT_EQ(expr->type, NODE_NUM_FLOAT);
    ASSERT(expr->data.num_float.value > 3.13 && expr->data.num_float.value < 3.15);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse read statement */
void test_parse_read(void) {
    TEST_START("test_parse_read");
    
    const char *program = "programa TEST\ninteiro x;\ninicio\nleia x;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 1);
    
    ASTNode *read = cmds->data.commands.items[0];
    ASSERT_EQ(read->type, NODE_READ);
    ASSERT_STR_EQ(read->data.read.var_name, "x");
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse write statement */
void test_parse_write(void) {
    TEST_START("test_parse_write");
    
    const char *program = "programa TEST\ninteiro x;\ninicio\nescreva 42;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 1);
    
    ASTNode *write = cmds->data.commands.items[0];
    ASSERT_EQ(write->type, NODE_WRITE);
    ASSERT(write->data.write.expr != NULL);
    ASSERT_EQ(write->data.write.expr->type, NODE_NUM_INT);
    ASSERT_EQ(write->data.write.expr->data.num_int.value, 42);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse writeln statement */
void test_parse_writeln(void) {
    TEST_START("test_parse_writeln");
    
    const char *program = "programa TEST\ninteiro x;\ninicio\nescreval 10;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 1);
    
    ASTNode *writeln = cmds->data.commands.items[0];
    ASSERT_EQ(writeln->type, NODE_WRITELN);
    ASSERT(writeln->data.write.expr != NULL);
    ASSERT_EQ(writeln->data.write.expr->type, NODE_NUM_INT);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse if statement */
void test_parse_if_statement(void) {
    TEST_START("test_parse_if_statement");
    
    const char *program = "programa TEST\ninteiro x;\ninicio\nse x > 5 entao\nx <- 10;\nfimse\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 1);
    
    ASTNode *if_stmt = cmds->data.commands.items[0];
    ASSERT_EQ(if_stmt->type, NODE_IF);
    ASSERT(if_stmt->data.if_stmt.condition != NULL);
    ASSERT_EQ(if_stmt->data.if_stmt.condition->type, NODE_BINOP);
    ASSERT_EQ(if_stmt->data.if_stmt.condition->data.binop.op, '>');
    ASSERT(if_stmt->data.if_stmt.then_branch != NULL);
    ASSERT_EQ(if_stmt->data.if_stmt.else_branch, NULL);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse if-else statement */
void test_parse_if_else_statement(void) {
    TEST_START("test_parse_if_else_statement");
    
    const char *program = "programa TEST\ninteiro x;\ninteiro y;\ninicio\nse x > 5 entao\ny <- 10;\nsenao\ny <- 20;\nfimse\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 1);
    
    ASTNode *if_stmt = cmds->data.commands.items[0];
    ASSERT_EQ(if_stmt->type, NODE_IF);
    ASSERT(if_stmt->data.if_stmt.condition != NULL);
    ASSERT(if_stmt->data.if_stmt.then_branch != NULL);
    ASSERT(if_stmt->data.if_stmt.else_branch != NULL);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse while loop */
void test_parse_while_loop(void) {
    TEST_START("test_parse_while_loop");
    
    const char *program = "programa TEST\ninteiro x;\ninicio\nenquanto x < 10 faca\nx <- x + 1;\nfimenquanto\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 1);
    
    ASTNode *while_loop = cmds->data.commands.items[0];
    ASSERT_EQ(while_loop->type, NODE_WHILE);
    ASSERT(while_loop->data.while_stmt.condition != NULL);
    ASSERT_EQ(while_loop->data.while_stmt.condition->type, NODE_BINOP);
    ASSERT(while_loop->data.while_stmt.body != NULL);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse for loop */
void test_parse_for_loop(void) {
    TEST_START("test_parse_for_loop");
    
    const char *program = "programa TEST\ninteiro i;\ninteiro x;\ninicio\npara i de 1 ate 10 passo 1 faca\nx <- x + i;\nfimpara\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 1);
    
    ASTNode *for_loop = cmds->data.commands.items[0];
    ASSERT_EQ(for_loop->type, NODE_FOR);
    ASSERT_STR_EQ(for_loop->data.for_stmt.var_name, "i");
    ASSERT(for_loop->data.for_stmt.start_expr != NULL);
    ASSERT(for_loop->data.for_stmt.end_expr != NULL);
    ASSERT(for_loop->data.for_stmt.step_expr != NULL);
    ASSERT(for_loop->data.for_stmt.body != NULL);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Parse nested statements */
void test_parse_nested_statements(void) {
    TEST_START("test_parse_nested_statements");
    
    const char *program = "programa TEST\ninteiro i;\ninteiro x;\ninicio\nse x > 0 entao\nenquanto x > 0 faca\nx <- x - 1;\nfimenquanto\nfimse\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 1);
    
    ASTNode *if_stmt = cmds->data.commands.items[0];
    ASSERT_EQ(if_stmt->type, NODE_IF);
    
    /* Check that then branch contains a while loop */
    ASTNode *then_cmds = if_stmt->data.if_stmt.then_branch;
    ASSERT(then_cmds != NULL);
    ASSERT_EQ(then_cmds->type, NODE_COMMANDS);
    ASSERT(then_cmds->data.commands.count > 0);
    
    ASTNode *while_loop = then_cmds->data.commands.items[0];
    ASSERT_EQ(while_loop->type, NODE_WHILE);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    
    TEST_END();
}

/* Test: Mixed statements */
void test_parse_mixed_statements(void) {
    TEST_START("test_parse_mixed_statements");
    
    const char *program = "programa TEST\ninteiro x;\ninteiro y;\ninicio\nx <- 5;\nleia y;\nescreva x + y;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 3);
    
    /* First: assignment */
    ASSERT_EQ(cmds->data.commands.items[0]->type, NODE_ASSIGNMENT);
    
    /* Second: read */
    ASSERT_EQ(cmds->data.commands.items[1]->type, NODE_READ);
    
    /* Third: write */
    ASSERT_EQ(cmds->data.commands.items[2]->type, NODE_WRITE);
    
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);

    TEST_END();
}

/* ========== TASK 9: Program Parsing Integration Tests ========== */

void test_parse_complete_empty_program(void) {
    TEST_START("test_parse_complete_empty_program");
    const char *program = "programa VAZIO\ninicio\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ASSERT_EQ(ast->type, NODE_PROGRAM);
    ASSERT_STR_EQ(ast->data.program.name, "VAZIO");
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_parse_program_with_one_declaration(void) {
    TEST_START("test_parse_program_with_one_declaration");
    const char *program = "programa UM\ninteiro valor;\ninicio\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ASTNode *decls = ast->data.program.declarations;
    ASSERT_EQ(decls->data.declarations.count, 1);
    ASSERT_STR_EQ(decls->data.declarations.items[0]->data.declaration.name, "valor");
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_parse_program_with_multiple_declarations(void) {
    TEST_START("test_parse_program_with_multiple_declarations");
    const char *program = "programa MULTI\ninteiro a;\nflutuante b;\ninteiro c;\nflutuante d;\ninicio\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ASTNode *decls = ast->data.program.declarations;
    ASSERT_EQ(decls->data.declarations.count, 4);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_parse_program_with_simple_statement(void) {
    TEST_START("test_parse_program_with_simple_statement");
    const char *program = "programa SIMPLE\ninteiro x;\ninicio\nx <- 42;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 1);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_parse_program_with_multiple_statements(void) {
    TEST_START("test_parse_program_with_multiple_statements");
    const char *program = "programa MULT\ninteiro x;\ninteiro y;\ninteiro z;\ninicio\nx <- 10;\ny <- 20;\nz <- x + y;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 3);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_parse_program_with_all_statement_types(void) {
    TEST_START("test_parse_program_with_all_statement_types");
    const char *program = "programa TODOSTXT\ninteiro x;\ninteiro y;\ninicio\nx <- 5;\nleia y;\nescreva x;\nescreval y;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 4);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_parse_program_with_nested_control_flow(void) {
    TEST_START("test_parse_program_with_nested_control_flow");
    const char *program = "programa NESTED\ninteiro x;\ninteiro i;\ninicio\nse x > 0 entao\nenquanto x > 0 faca\nx <- x - 1;\nfimenquanto\nfimse\npara i de 1 ate 10 passo 1 faca\nescreva i;\nfimpara\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 2);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_parse_program_with_complex_expressions(void) {
    TEST_START("test_parse_program_with_complex_expressions");
    const char *program = "programa COMPLEX\ninteiro a;\ninteiro b;\ninteiro c;\ninteiro result;\ninicio\nresult <- a + b * c - (a / b);\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 1);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

/* ========== TASK 10: Comprehensive Parser Test Suite ========== */

void test_empty_declarations(void) {
    TEST_START("test_empty_declarations");
    const char *program = "programa NODECL\ninteiro a;\ninteiro b;\ninteiro c;\ninicio\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 0);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_complex_nested_expressions(void) {
    TEST_START("test_complex_nested_expressions");
    const char *program = "programa EXPR\ninteiro x;\ninteiro y;\ninteiro z;\ninicio\nz <- (x + y) * (x - y) / (x + 1);\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_all_operators(void) {
    TEST_START("test_all_operators");
    const char *program = "programa OPS\ninteiro x;\ninteiro y;\ninteiro a;\ninteiro b;\ninteiro c;\ninteiro d;\ninteiro e;\ninteiro f;\ninicio\na <- x + y;\nb <- x - y;\nc <- x * y;\nd <- x / y;\ne <- x > y;\nf <- x = y;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ASTNode *cmds = ast->data.program.commands;
    ASSERT_EQ(cmds->data.commands.count, 6);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_operator_precedence_all(void) {
    TEST_START("test_operator_precedence_all");
    const char *program = "programa PREC\ninteiro x;\ninteiro result;\ninicio\nresult <- 2 + 3 * 4 - 5 / 2;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_parentheses_override_precedence(void) {
    TEST_START("test_parentheses_override_precedence");
    const char *program = "programa PAREN\ninteiro x;\ninteiro result;\ninicio\nresult <- (2 + 3) * (4 - 1);\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_unary_in_expressions(void) {
    TEST_START("test_unary_in_expressions");
    const char *program = "programa UNARY\ninteiro x;\ninteiro y;\ninteiro z;\ninicio\nx <- -5;\ny <- -x + 10;\nz <- -(x + y);\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_assignment_with_complex_expr(void) {
    TEST_START("test_assignment_with_complex_expr");
    const char *program = "programa ASGN\ninteiro a;\ninteiro b;\ninteiro c;\ninteiro result;\ninicio\nresult <- (a + b) * c - a / (b + 1);\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_read_statement(void) {
    TEST_START("test_read_statement");
    const char *program = "programa RD\ninteiro x;\ninteiro y;\ninteiro z;\ninicio\nleia x;\nleia y;\nleia z;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_write_statement(void) {
    TEST_START("test_write_statement");
    const char *program = "programa WR\ninteiro x;\ninicio\nescreva x;\nescreva x + 5;\nescreva 42;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_writeln_statement(void) {
    TEST_START("test_writeln_statement");
    const char *program = "programa WRL\ninteiro x;\ninicio\nescreval x;\nescreval x * 2;\nescreval 100;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_if_statement_without_else(void) {
    TEST_START("test_if_statement_without_else");
    const char *program = "programa IF1\ninteiro x;\ninteiro y;\ninicio\nse x > 5 entao\ny <- 10;\nfimse\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_if_statement_with_else(void) {
    TEST_START("test_if_statement_with_else");
    const char *program = "programa IF2\ninteiro x;\ninteiro y;\ninteiro z;\ninicio\nse x > 5 entao\ny <- 10;\nsenao\nz <- 20;\nfimse\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_nested_if_statements(void) {
    TEST_START("test_nested_if_statements");
    const char *program = "programa IFNEST\ninteiro x;\ninteiro y;\ninteiro a;\ninteiro b;\ninicio\nse x > 0 entao\nse y > 0 entao\na <- 1;\nfimse\nfimse\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_while_loop(void) {
    TEST_START("test_while_loop");
    const char *program = "programa WHL\ninteiro x;\ninteiro sum;\ninicio\nsum <- 0;\nenquanto x > 0 faca\nsum <- sum + x;\nx <- x - 1;\nfimenquanto\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_for_loop_all_variations(void) {
    TEST_START("test_for_loop_all_variations");
    const char *program = "programa FOR\ninteiro i;\ninteiro sum;\ninicio\npara i de 1 ate 10 passo 1 faca\nsum <- sum + i;\nfimpara\npara i de 10 ate 1 passo -1 faca\nsum <- sum - i;\nfimpara\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_nested_loops(void) {
    TEST_START("test_nested_loops");
    const char *program = "programa NESTED_LOOPS\ninteiro i;\ninteiro j;\ninteiro sum;\ninicio\npara i de 1 ate 5 passo 1 faca\npara j de 1 ate 5 passo 1 faca\nsum <- sum + 1;\nfimpara\nfimpara\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_statements_with_all_operators(void) {
    TEST_START("test_statements_with_all_operators");
    const char *program = "programa OPR\ninteiro x;\ninteiro y;\ninteiro r1;\ninteiro r2;\ninteiro r3;\ninteiro r4;\ninteiro r5;\ninteiro r6;\ninicio\nr1 <- x + y;\nr2 <- x - y;\nr3 <- x * y;\nr4 <- x / y;\nr5 <- x > y;\nr6 <- x = y;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_if_inside_while(void) {
    TEST_START("test_if_inside_while");
    const char *program = "programa IFW\ninteiro x;\ninteiro y;\ninicio\nenquanto x > 0 faca\nse x > 5 entao\ny <- 10;\nfimse\nx <- x - 1;\nfimenquanto\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_while_inside_for(void) {
    TEST_START("test_while_inside_for");
    const char *program = "programa WIF\ninteiro i;\ninteiro x;\ninicio\npara i de 1 ate 10 passo 1 faca\nenquanto x > 0 faca\nx <- x - 1;\nfimenquanto\nfimpara\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_for_inside_if(void) {
    TEST_START("test_for_inside_if");
    const char *program = "programa FIF\ninteiro i;\ninteiro x;\ninteiro y;\ninicio\nse x > 0 entao\npara i de 1 ate 5 passo 1 faca\ny <- y + 1;\nfimpara\nfimse\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_deeply_nested_structures(void) {
    TEST_START("test_deeply_nested_structures");
    const char *program = "programa DEEP\ninteiro i;\ninteiro j;\ninteiro k;\ninteiro x;\ninicio\npara i de 1 ate 3 passo 1 faca\nse i > 0 entao\npara j de 1 ate 3 passo 1 faca\nenquanto x > 0 faca\nk <- k + 1;\nfimenquanto\nfimpara\nfimse\nfimpara\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_mixed_nesting_patterns(void) {
    TEST_START("test_mixed_nesting_patterns");
    const char *program = "programa MIX\ninteiro i;\ninteiro x;\ninteiro y;\ninteiro z;\ninicio\nse x > 0 entao\nenquanto y > 0 faca\npara i de 1 ate 10 passo 1 faca\nz <- z + 1;\nfimpara\ny <- y - 1;\nfimenquanto\nfimse\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_error_missing_programa(void) {
    TEST_START("test_error_missing_programa");
    const char *program = "inteiro x;\ninicio\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast == NULL);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_error_missing_initialization(void) {
    TEST_START("test_error_missing_initialization");
    const char *program = "programa TEST\ninteiro x;\ninicio";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast == NULL);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_error_missing_fim(void) {
    TEST_START("test_error_missing_fim");
    const char *program = "programa TEST\ninteiro x;\ninicio\n";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast == NULL);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_error_missing_period(void) {
    TEST_START("test_error_missing_period");
    const char *program = "programa TEST\ninteiro x;\ninicio\nfim";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast == NULL);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_error_invalid_declaration(void) {
    TEST_START("test_error_invalid_declaration");
    const char *program = "programa TEST\ninteiro;\ninicio\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast == NULL);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_error_incomplete_statement(void) {
    TEST_START("test_error_incomplete_statement");
    const char *program = "programa TEST\ninteiro x;\ninicio\nx <-;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast == NULL);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_error_missing_then(void) {
    TEST_START("test_error_missing_then");
    const char *program = "programa TEST\ninteiro x;\ninicio\nse x > 5\nx <- 10;\nfimse\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast == NULL);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_error_missing_faca(void) {
    TEST_START("test_error_missing_faca");
    const char *program = "programa TEST\ninteiro x;\ninicio\nenquanto x > 0\nx <- x - 1;\nfimenquanto\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast == NULL);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_error_unmatched_parenthesis(void) {
    TEST_START("test_error_unmatched_parenthesis");
    const char *program = "programa TEST\ninteiro x;\ninteiro y;\ninicio\ny <- (x + 5;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast == NULL);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_parser_memory_cleanup(void) {
    TEST_START("test_parser_memory_cleanup");
    const char *program = "programa MEM\ninteiro x;\ninteiro y;\ninteiro z;\ninicio\nx <- 5;\ny <- 10;\nz <- x + y;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_ast_cleanup_after_parse(void) {
    TEST_START("test_ast_cleanup_after_parse");
    const char *program = "programa CLEAN\ninteiro a;\ninteiro b;\ninteiro c;\ninteiro d;\ninteiro e;\ninicio\na <- 1;\nb <- 2;\nc <- a + b;\nd <- c * 2;\ne <- d - 1;\nfim.";
    TokenList *tokens = lexer_tokenize(program);
    ASSERT(tokens != NULL);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse_program(parser);
    ASSERT(ast != NULL);
    ASSERT_EQ(ast->type, NODE_PROGRAM);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    TEST_END();
}

void test_large_program_memory(void) {
    TEST_START("test_large_program_memory");
    char program[2048];
    strcpy(program, "programa LARGE\n");
    for (int i = 0; i < 5; i++) {
        if (i % 2 == 0) strcat(program, "inteiro var");
        else strcat(program, "flutuante var");
        char num[20];
        sprintf(num, "%d", i);
        strcat(program, num);
        strcat(program, ";\n");
    }
    strcat(program, "inicio\nfim.");
    TokenList *tokens = lexer_tokenize(program);
    if (tokens != NULL) {
        Parser *parser = parser_create(tokens);
        ASTNode *ast = parser_parse_program(parser);
        if (ast != NULL) {
            ASSERT_EQ(ast->type, NODE_PROGRAM);
            ast_free(ast);
        }
        parser_free(parser);
        token_list_free(tokens);
    }
    TEST_END();
}

/* Main test runner */
int main(void) {
    printf("========================================\n");
    printf("  PARSER TEST SUITE\n");
    printf("========================================\n");
    
    test_create_parser();
    test_parse_empty_program();
    test_parse_simple_declaration();
    test_parse_multiple_declarations();
    test_parse_program_structure();
    test_parser_helpers();
    test_error_handling();
    test_declarations_only();
    test_mixed_types();
    test_complex_program();
    test_parse_simple_number();
    test_parse_identifier();
    test_parse_binary_operator();
    test_parse_operator_precedence();
    test_parse_unary_operator();
    test_parse_parenthesized_expression();
    test_parse_complex_expression();
    test_parse_assignment();
    test_parse_left_associativity();
    test_parse_comparison_operators();
    test_parse_float_literal();
    test_parse_read();
    test_parse_write();
    test_parse_writeln();
    test_parse_if_statement();
    test_parse_if_else_statement();
    test_parse_while_loop();
    test_parse_for_loop();
    test_parse_nested_statements();
    test_parse_mixed_statements();
    
    /* Task 9: Program Parsing Integration Tests */
    test_parse_complete_empty_program();
    test_parse_program_with_one_declaration();
    test_parse_program_with_multiple_declarations();
    test_parse_program_with_simple_statement();
    test_parse_program_with_multiple_statements();
    test_parse_program_with_all_statement_types();
    test_parse_program_with_nested_control_flow();
    test_parse_program_with_complex_expressions();
    
    /* Task 10: Comprehensive Parser Test Suite */
    test_empty_declarations();
    test_complex_nested_expressions();
    test_all_operators();
    test_operator_precedence_all();
    test_parentheses_override_precedence();
    test_unary_in_expressions();
    test_assignment_with_complex_expr();
    test_read_statement();
    test_write_statement();
    test_writeln_statement();
    test_if_statement_without_else();
    test_if_statement_with_else();
    test_nested_if_statements();
    test_while_loop();
    test_for_loop_all_variations();
    test_nested_loops();
    test_statements_with_all_operators();
    test_if_inside_while();
    test_while_inside_for();
    test_for_inside_if();
    test_deeply_nested_structures();
    test_mixed_nesting_patterns();
    test_error_missing_programa();
    test_error_missing_initialization();
    test_error_missing_fim();
    test_error_missing_period();
    test_error_invalid_declaration();
    test_error_incomplete_statement();
    test_error_missing_then();
    test_error_missing_faca();
    test_error_unmatched_parenthesis();
    test_parser_memory_cleanup();
    test_ast_cleanup_after_parse();
    test_large_program_memory();
    
    printf("\n========================================\n");
    printf("  TEST RESULTS\n");
    printf("========================================\n");
    printf("Tests run: %d / %d\n", test_passed, test_count);
    printf("Assertions passed: %d / %d\n", assertions_passed, assertions);
    printf("========================================\n");
    
    return (assertions_passed == assertions) ? 0 : 1;
}
