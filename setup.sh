#!/bin/bash
# This script will set up the project structure

# Create directories
mkdir -p src tests unity

# Create src/token.h
cat > src/token.h << 'EOF'
#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

typedef enum {
    /* Keywords (27) */
    TOK_PROGRAMA,
    TOK_INICIO,
    TOK_FIM,
    TOK_INTEIRO,
    TOK_FLUTUANTE,
    TOK_VAZIO,
    TOK_SE,
    TOK_ENTAO,
    TOK_SENAO,
    TOK_FIMSE,
    TOK_ENQUANTO,
    TOK_FIMENQUANTO,
    TOK_PARA,
    TOK_DE,
    TOK_ATE,
    TOK_PASSO,
    TOK_FACA,
    TOK_FIMPARA,
    TOK_LEIA,
    TOK_ESCREVA,
    TOK_ESCREVAL,
    TOK_E,
    TOK_OU,
    TOK_NAO,
    TOK_DIV,
    TOK_PROCEDIMENTO,
    TOK_RETORNA,
    
    /* Operators (14) */
    TOK_ATRIB,          /* <- */
    TOK_PLUS,           /* + */
    TOK_MINUS,          /* - */
    TOK_MULT,           /* * */
    TOK_GT,             /* > */
    TOK_LT,             /* < */
    TOK_EQ,             /* = */
    TOK_NEQ,            /* <> */
    TOK_GTE,            /* >= */
    TOK_LTE,            /* <= */
    TOK_LPAREN,         /* ( */
    TOK_RPAREN,         /* ) */
    TOK_COMMA,          /* , */
    TOK_SEMICOLON,      /* ; */
    
    /* Literals and Identifiers (3) */
    TOK_ID,
    TOK_NUM_INT,
    TOK_NUM_FLOAT,
    
    /* Special (2) */
    TOK_EOF,
    TOK_ERROR
} TokenType;

typedef union {
    int int_value;
    float float_value;
} TokenValue;

typedef struct {
    TokenType type;
    char *lexeme;
    size_t lexeme_len;
    int line;
    int column;
    TokenValue value;
} Token;

typedef struct {
    Token *tokens;
    size_t count;
    size_t capacity;
} TokenList;

const char *token_type_name(TokenType type);

void token_list_free(TokenList *list);

#endif /* TOKEN_H */
EOF

# Create tests/test_lexer.c
cat > tests/test_lexer.c << 'EOF'
#include "unity.h"
#include "../src/lexer.h"

void setUp(void) {
    /* Empty setup for now */
}

void tearDown(void) {
    /* Empty teardown for now */
}

void test_lexer_single_integer(void) {
    TokenList *tokens = lexer_tokenize("42");
    
    TEST_ASSERT_NOT_NULL(tokens);
    TEST_ASSERT_EQUAL_INT(2, tokens->count);
    
    /* First token should be NUM_INT */
    TEST_ASSERT_EQUAL_INT(TOK_NUM_INT, tokens->tokens[0].type);
    TEST_ASSERT_EQUAL_INT(42, tokens->tokens[0].value.int_value);
    
    /* Second token should be EOF */
    TEST_ASSERT_EQUAL_INT(TOK_EOF, tokens->tokens[1].type);
    
    token_list_free(tokens);
}

int main(void) {
    return unity_main();
}
EOF

echo "Project setup complete!"
echo "Files created:"
echo "- src/token.h"
echo "- tests/test_lexer.c"
echo "- Makefile (already created)"
