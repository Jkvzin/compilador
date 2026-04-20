#!/usr/bin/env python3
"""Setup Task 2: Create lexer implementation files"""
import os
import sys

# Change to repo root
repo_root = os.path.dirname(os.path.abspath(__file__))
os.chdir(repo_root)

# Create directories
os.makedirs("src", exist_ok=True)
os.makedirs("tests", exist_ok=True)

print(f"Working directory: {os.getcwd()}")
print(f"src exists: {os.path.exists('src')}")
print(f"tests exists: {os.path.exists('tests')}")

# Create lexer.h
lexer_h_content = '''#ifndef LEXER_H
#define LEXER_H

#include "token.h"

typedef struct {
    const char *input;
    size_t current;
    size_t start;
    int line;
    int column;
} Lexer;

Lexer *lexer_create(const char *input);
void lexer_free(Lexer *lexer);
TokenList *lexer_tokenize(const char *input);

#endif
'''

with open("src/lexer.h", "w") as f:
    f.write(lexer_h_content)
print("✓ Created src/lexer.h")

# Create lexer.c
lexer_c_content = '''#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Keyword lookup table */
static const struct {
    const char *lexeme;
    TokenType type;
} KEYWORDS[] = {
    {"programa", TOK_PROGRAMA},
    {"inicio", TOK_INICIO},
    {"fim", TOK_FIM},
    {"inteiro", TOK_INTEIRO},
    {"flutuante", TOK_FLUTUANTE},
    {"vazio", TOK_VAZIO},
    {"se", TOK_SE},
    {"entao", TOK_ENTAO},
    {"senao", TOK_SENAO},
    {"fimse", TOK_FIMSE},
    {"enquanto", TOK_ENQUANTO},
    {"fimenquanto", TOK_FIMENQUANTO},
    {"para", TOK_PARA},
    {"de", TOK_DE},
    {"ate", TOK_ATE},
    {"passo", TOK_PASSO},
    {"faca", TOK_FACA},
    {"fimpara", TOK_FIMPARA},
    {"leia", TOK_LEIA},
    {"escreva", TOK_ESCREVA},
    {"escreval", TOK_ESCREVAL},
    {"e", TOK_E},
    {"ou", TOK_OU},
    {"nao", TOK_NAO},
    {"div", TOK_DIV},
    {"procedimento", TOK_PROCEDIMENTO},
    {"retorna", TOK_RETORNA},
    {NULL, TOK_ERROR}
};

Lexer *lexer_create(const char *input) {
    Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
    if (!lexer) return NULL;
    
    lexer->input = input;
    lexer->current = 0;
    lexer->start = 0;
    lexer->line = 1;
    lexer->column = 1;
    
    return lexer;
}

void lexer_free(Lexer *lexer) {
    free(lexer);
}

static char peek(Lexer *lexer) {
    if (lexer->current >= strlen(lexer->input)) {
        return '\\0';
    }
    return lexer->input[lexer->current];
}

static char peek_next(Lexer *lexer) {
    size_t next = lexer->current + 1;
    if (next >= strlen(lexer->input)) {
        return '\\0';
    }
    return lexer->input[next];
}

static void advance(Lexer *lexer) {
    if (lexer->current < strlen(lexer->input)) {
        if (lexer->input[lexer->current] == '\\n') {
            lexer->line++;
            lexer->column = 1;
        } else {
            lexer->column++;
        }
        lexer->current++;
    }
}

static void skip_whitespace(Lexer *lexer) {
    while (isspace(peek(lexer))) {
        advance(lexer);
    }
}

static Token make_token(Lexer *lexer, TokenType type) {
    Token token;
    token.type = type;
    token.line = lexer->line;
    token.column = lexer->column;
    token.lexeme_len = lexer->current - lexer->start;
    token.lexeme = (char *)malloc(token.lexeme_len + 1);
    
    if (token.lexeme) {
        strncpy(token.lexeme, lexer->input + lexer->start, token.lexeme_len);
        token.lexeme[token.lexeme_len] = '\\0';
    }
    
    return token;
}

static Token scan_number(Lexer *lexer) {
    int line = lexer->line;
    int column = lexer->column;
    lexer->start = lexer->current;
    
    /* Scan integer part */
    while (isdigit(peek(lexer))) {
        advance(lexer);
    }
    
    /* Check for float */
    if (peek(lexer) == '.' && isdigit(peek_next(lexer))) {
        advance(lexer); /* consume '.' */
        while (isdigit(peek(lexer))) {
            advance(lexer);
        }
        
        Token token = make_token(lexer, TOK_NUM_FLOAT);
        token.line = line;
        token.column = column;
        token.value.float_value = atof(token.lexeme);
        return token;
    }
    
    Token token = make_token(lexer, TOK_NUM_INT);
    token.line = line;
    token.column = column;
    token.value.int_value = atoi(token.lexeme);
    return token;
}

static Token scan_identifier(Lexer *lexer) {
    int line = lexer->line;
    int column = lexer->column;
    lexer->start = lexer->current;
    
    while (isalnum(peek(lexer)) || peek(lexer) == '_') {
        advance(lexer);
    }
    
    Token token = make_token(lexer, TOK_ID);
    token.line = line;
    token.column = column;
    
    /* Check if it's a keyword */
    for (int i = 0; KEYWORDS[i].lexeme != NULL; i++) {
        if (strlen(KEYWORDS[i].lexeme) == token.lexeme_len &&
            strncmp(token.lexeme, KEYWORDS[i].lexeme, token.lexeme_len) == 0) {
            token.type = KEYWORDS[i].type;
            break;
        }
    }
    
    return token;
}

static Token scan_operator(Lexer *lexer) {
    int line = lexer->line;
    int column = lexer->column;
    lexer->start = lexer->current;
    
    char c = peek(lexer);
    advance(lexer);
    
    /* Check for two-character operators */
    if (c == '<') {
        if (peek(lexer) == '-') {
            advance(lexer);
            Token token = make_token(lexer, TOK_ATRIB);
            token.line = line;
            token.column = column;
            return token;
        } else if (peek(lexer) == '>') {
            advance(lexer);
            Token token = make_token(lexer, TOK_NEQ);
            token.line = line;
            token.column = column;
            return token;
        } else if (peek(lexer) == '=') {
            advance(lexer);
            Token token = make_token(lexer, TOK_LTE);
            token.line = line;
            token.column = column;
            return token;
        }
        Token token = make_token(lexer, TOK_LT);
        token.line = line;
        token.column = column;
        return token;
    } else if (c == '>' && peek(lexer) == '=') {
        advance(lexer);
        Token token = make_token(lexer, TOK_GTE);
        token.line = line;
        token.column = column;
        return token;
    }
    
    /* Single-character operators */
    TokenType type;
    switch (c) {
        case '+': type = TOK_PLUS; break;
        case '-': type = TOK_MINUS; break;
        case '*': type = TOK_MULT; break;
        case '>': type = TOK_GT; break;
        case '=': type = TOK_EQ; break;
        case '(': type = TOK_LPAREN; break;
        case ')': type = TOK_RPAREN; break;
        case ',': type = TOK_COMMA; break;
        case ';': type = TOK_SEMICOLON; break;
        default: type = TOK_ERROR;
    }
    
    Token token = make_token(lexer, type);
    token.line = line;
    token.column = column;
    return token;
}

TokenList *lexer_tokenize(const char *input) {
    if (!input) return NULL;
    
    TokenList *list = (TokenList *)malloc(sizeof(TokenList));
    if (!list) return NULL;
    
    list->capacity = 16;
    list->count = 0;
    list->tokens = (Token *)malloc(sizeof(Token) * list->capacity);
    
    if (!list->tokens) {
        free(list);
        return NULL;
    }
    
    Lexer *lexer = lexer_create(input);
    if (!lexer) {
        free(list->tokens);
        free(list);
        return NULL;
    }
    
    while (peek(lexer) != '\\0') {
        skip_whitespace(lexer);
        
        if (peek(lexer) == '\\0') break;
        
        Token token;
        if (isdigit(peek(lexer))) {
            token = scan_number(lexer);
        } else if (isalpha(peek(lexer)) || peek(lexer) == '_') {
            token = scan_identifier(lexer);
        } else {
            token = scan_operator(lexer);
        }
        
        /* Resize if needed */
        if (list->count >= list->capacity) {
            list->capacity *= 2;
            Token *new_tokens = (Token *)realloc(list->tokens, sizeof(Token) * list->capacity);
            if (!new_tokens) {
                free(lexer);
                token_list_free(list);
                return NULL;
            }
            list->tokens = new_tokens;
        }
        
        list->tokens[list->count++] = token;
    }
    
    /* Add EOF token */
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        Token *new_tokens = (Token *)realloc(list->tokens, sizeof(Token) * list->capacity);
        if (!new_tokens) {
            free(lexer);
            token_list_free(list);
            return NULL;
        }
        list->tokens = new_tokens;
    }
    
    Token eof_token;
    eof_token.type = TOK_EOF;
    eof_token.line = lexer->line;
    eof_token.column = lexer->column;
    eof_token.lexeme = (char *)malloc(1);
    if (eof_token.lexeme) {
        eof_token.lexeme[0] = '\\0';
    }
    eof_token.lexeme_len = 0;
    eof_token.value.int_value = 0;
    
    list->tokens[list->count++] = eof_token;
    
    lexer_free(lexer);
    return list;
}

void token_list_free(TokenList *list) {
    if (!list) return;
    
    if (list->tokens) {
        for (size_t i = 0; i < list->count; i++) {
            free(list->tokens[i].lexeme);
        }
        free(list->tokens);
    }
    
    free(list);
}
'''

with open("src/lexer.c", "w") as f:
    f.write(lexer_c_content)
print("✓ Created src/lexer.c")

# Move token.h and test_lexer.c if they exist at root
if os.path.exists("token.h") and not os.path.exists("src/token.h"):
    with open("token.h", "r") as f:
        content = f.read()
    with open("src/token.h", "w") as f:
        f.write(content)
    print("✓ Copied token.h to src/")

if os.path.exists("test_lexer.c") and not os.path.exists("tests/test_lexer.c"):
    with open("test_lexer.c", "r") as f:
        content = f.read()
    with open("tests/test_lexer.c", "w") as f:
        f.write(content)
    print("✓ Copied test_lexer.c to tests/")

print("\n✓ Task 2 setup complete!")
