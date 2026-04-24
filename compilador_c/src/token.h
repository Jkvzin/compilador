#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

typedef enum {
    /* Keywords (28) */
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
    TOK_ESCREVERLN,
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
    TOK_PERIOD,         /* . */
    
    /* Literals and Identifiers (3) */
    TOK_ID,
    TOK_NUM_INT,
    TOK_NUM_FLOAT,
    
    /* Special (2) */
    TOK_EOF,
    TOK_ERROR,
    
    /* End marker for array sizing */
    TOK_COUNT
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
