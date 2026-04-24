#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/lexer.h"

/* Simple test framework */
int tests_passed = 0;
int tests_failed = 0;

void assert_equal_int(int expected, int actual, const char *test_name) {
    if (expected == actual) {
        printf("✓ %s\n", test_name);
        tests_passed++;
    } else {
        printf("✗ %s (expected %d, got %d)\n", test_name, expected, actual);
        tests_failed++;
    }
}

void assert_not_null(void *ptr, const char *test_name) {
    if (ptr != NULL) {
        printf("✓ %s\n", test_name);
        tests_passed++;
    } else {
        printf("✗ %s (pointer is NULL)\n", test_name);
        tests_failed++;
    }
}

void assert_equal_token_type(TokenType expected, TokenType actual, const char *test_name) {
    if (expected == actual) {
        printf("✓ %s\n", test_name);
        tests_passed++;
    } else {
        printf("✗ %s (expected %s, got %s)\n", test_name, 
               token_type_name(expected), token_type_name(actual));
        tests_failed++;
    }
}

/* Test: Single integer */
void test_lexer_single_integer(void) {
    printf("\nTest: Single Integer\n");
    TokenList *tokens = lexer_tokenize("42");
    
    assert_not_null(tokens, "  tokens not null");
    assert_equal_int(2, tokens->count, "  token count == 2 (42 + EOF)");
    
    if (tokens->count >= 1) {
        assert_equal_token_type(TOK_NUM_INT, tokens->tokens[0].type, 
                               "  first token is TOK_NUM_INT");
        assert_equal_int(42, tokens->tokens[0].value.int_value,
                        "  integer value == 42");
    }
    
    if (tokens->count >= 2) {
        assert_equal_token_type(TOK_EOF, tokens->tokens[1].type,
                               "  second token is TOK_EOF");
    }
    
    token_list_free(tokens);
}

/* Test: Operator recognition */
void test_lexer_operators(void) {
    printf("\nTest: Operators\n");
    TokenList *tokens = lexer_tokenize("+ - * ( )");
    
    assert_not_null(tokens, "  tokens not null");
    
    if (tokens->count >= 6) {
        assert_equal_token_type(TOK_PLUS, tokens->tokens[0].type, "  first is +");
        assert_equal_token_type(TOK_MINUS, tokens->tokens[1].type, "  second is -");
        assert_equal_token_type(TOK_MULT, tokens->tokens[2].type, "  third is *");
        assert_equal_token_type(TOK_LPAREN, tokens->tokens[3].type, "  fourth is (");
        assert_equal_token_type(TOK_RPAREN, tokens->tokens[4].type, "  fifth is )");
        assert_equal_token_type(TOK_EOF, tokens->tokens[5].type, "  last is EOF");
    }
    
    token_list_free(tokens);
}

/* Test: Keyword recognition */
void test_lexer_keywords(void) {
    printf("\nTest: Keywords\n");
    TokenList *tokens = lexer_tokenize("programa inicio fim");
    
    assert_not_null(tokens, "  tokens not null");
    
    if (tokens->count >= 4) {
        assert_equal_token_type(TOK_PROGRAMA, tokens->tokens[0].type, 
                               "  first is programa");
        assert_equal_token_type(TOK_INICIO, tokens->tokens[1].type,
                               "  second is inicio");
        assert_equal_token_type(TOK_FIM, tokens->tokens[2].type,
                               "  third is fim");
        assert_equal_token_type(TOK_EOF, tokens->tokens[3].type,
                               "  last is EOF");
    }
    
    token_list_free(tokens);
}

/* Test: Identifiers */
void test_lexer_identifiers(void) {
    printf("\nTest: Identifiers\n");
    TokenList *tokens = lexer_tokenize("x contador _temp");
    
    assert_not_null(tokens, "  tokens not null");
    
    if (tokens->count >= 4) {
        assert_equal_token_type(TOK_ID, tokens->tokens[0].type, "  first is ID");
        assert_equal_token_type(TOK_ID, tokens->tokens[1].type, "  second is ID");
        assert_equal_token_type(TOK_ID, tokens->tokens[2].type, "  third is ID");
        assert_equal_token_type(TOK_EOF, tokens->tokens[3].type, "  last is EOF");
    }
    
    token_list_free(tokens);
}

/* Test: Assignment operator */
void test_lexer_assignment(void) {
    printf("\nTest: Assignment Operator\n");
    TokenList *tokens = lexer_tokenize("x <- 5");
    
    assert_not_null(tokens, "  tokens not null");
    
    if (tokens->count >= 4) {
        assert_equal_token_type(TOK_ID, tokens->tokens[0].type, "  first is ID");
        assert_equal_token_type(TOK_ATRIB, tokens->tokens[1].type, "  second is <-");
        assert_equal_token_type(TOK_NUM_INT, tokens->tokens[2].type, "  third is number");
        assert_equal_token_type(TOK_EOF, tokens->tokens[3].type, "  last is EOF");
    }
    
    token_list_free(tokens);
}

/* Test: Comparison operators */
void test_lexer_comparisons(void) {
    printf("\nTest: Comparison Operators\n");
    TokenList *tokens = lexer_tokenize("< > = <> <= >=");
    
    assert_not_null(tokens, "  tokens not null");
    
    if (tokens->count >= 7) {
        assert_equal_token_type(TOK_LT, tokens->tokens[0].type, "  first is <");
        assert_equal_token_type(TOK_GT, tokens->tokens[1].type, "  second is >");
        assert_equal_token_type(TOK_EQ, tokens->tokens[2].type, "  third is =");
        assert_equal_token_type(TOK_NEQ, tokens->tokens[3].type, "  fourth is <>");
        assert_equal_token_type(TOK_LTE, tokens->tokens[4].type, "  fifth is <=");
        assert_equal_token_type(TOK_GTE, tokens->tokens[5].type, "  sixth is >=");
        assert_equal_token_type(TOK_EOF, tokens->tokens[6].type, "  last is EOF");
    }
    
    token_list_free(tokens);
}

/* Main test runner */
int main(void) {
    printf("\n");
    printf("════════════════════════════════════════\n");
    printf("      SIMPLES Lexer Test Suite\n");
    printf("════════════════════════════════════════\n");
    
    /* Run all tests */
    test_lexer_single_integer();
    test_lexer_operators();
    test_lexer_keywords();
    test_lexer_identifiers();
    test_lexer_assignment();
    test_lexer_comparisons();
    
    /* Print summary */
    printf("\n");
    printf("════════════════════════════════════════\n");
    printf("TEST RESULTS\n");
    printf("════════════════════════════════════════\n");
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    printf("Total:  %d\n", tests_passed + tests_failed);
    printf("════════════════════════════════════════\n");
    printf("\n");
    
    return tests_failed == 0 ? 0 : 1;
}
