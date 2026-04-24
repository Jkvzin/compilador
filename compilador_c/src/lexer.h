#ifndef LEXER_H
#define LEXER_H

#include "token.h"

typedef struct {
    const char *input;
    size_t current;
    size_t start;
    int line;
    int column;
} Lexer;

// Lexer functions
Lexer *lexer_create(const char *input);
void lexer_free(Lexer *lexer);

// Tokenization
TokenList *lexer_tokenize(const char *input);
void token_list_free(TokenList *list);

#endif
