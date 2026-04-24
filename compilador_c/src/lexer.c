#include "lexer.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Keyword lookup table
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
    {"escreverln", TOK_ESCREVERLN},
    {"e", TOK_E},
    {"ou", TOK_OU},
    {"nao", TOK_NAO},
    {"div", TOK_DIV},
    {"procedimento", TOK_PROCEDIMENTO},
    {"retorna", TOK_RETORNA},
    {NULL, TOK_ERROR}
};

Lexer *lexer_create(const char *input) {
    Lexer *lexer = malloc(sizeof(Lexer));
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
    if (lexer->current >= strlen(lexer->input)) return '\0';
    return lexer->input[lexer->current];
}

static char peek_next(Lexer *lexer) {
    if (lexer->current + 1 >= strlen(lexer->input)) return '\0';
    return lexer->input[lexer->current + 1];
}

static char advance(Lexer *lexer) {
    char c = peek(lexer);
    lexer->current++;
    if (c == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    return c;
}

static void skip_whitespace(Lexer *lexer) {
    while (isspace(peek(lexer))) {
        advance(lexer);
    }
}

/* make_token now does strdup so each token owns its lexeme string */
static Token make_token(Lexer *lexer, TokenType type) {
    Token token;
    token.type = type;
    size_t len = lexer->current - lexer->start;
    token.lexeme = malloc(len + 1);
    if (token.lexeme) {
        memcpy(token.lexeme, lexer->input + lexer->start, len);
        token.lexeme[len] = '\0';
    }
    token.lexeme_len = len;
    token.line = lexer->line;
    token.column = lexer->column - (int)len;
    token.value.int_value = 0;
    return token;
}

static Token scan_number(Lexer *lexer) {
    lexer->start = lexer->current;
    
    while (isdigit(peek(lexer))) {
        advance(lexer);
    }
    
    // Check for float
    if (peek(lexer) == '.' && isdigit(peek_next(lexer))) {
        advance(lexer);  // consume '.'
        while (isdigit(peek(lexer))) {
            advance(lexer);
        }
        Token token = make_token(lexer, TOK_NUM_FLOAT);
        token.value.float_value = (float)atof(token.lexeme);
        return token;
    }
    
    Token token = make_token(lexer, TOK_NUM_INT);
    token.value.int_value = atoi(token.lexeme);
    return token;
}

static Token scan_identifier(Lexer *lexer) {
    lexer->start = lexer->current;
    
    while (isalnum(peek(lexer)) || peek(lexer) == '_') {
        advance(lexer);
    }
    
    Token token = make_token(lexer, TOK_ID);
    
    // Check if identifier is a keyword
    for (int i = 0; KEYWORDS[i].lexeme != NULL; i++) {
        if (token.lexeme_len == strlen(KEYWORDS[i].lexeme) &&
            strncmp(token.lexeme, KEYWORDS[i].lexeme, token.lexeme_len) == 0) {
            token.type = KEYWORDS[i].type;
            break;
        }
    }
    
    return token;
}

static Token scan_operator(Lexer *lexer) {
    lexer->start = lexer->current;
    char c = advance(lexer);
    
    switch (c) {
        case '+': return make_token(lexer, TOK_PLUS);
        case '*': return make_token(lexer, TOK_MULT);
        case '(': return make_token(lexer, TOK_LPAREN);
        case ')': return make_token(lexer, TOK_RPAREN);
        case ',': return make_token(lexer, TOK_COMMA);
        case ';': return make_token(lexer, TOK_SEMICOLON);
        case '.': return make_token(lexer, TOK_PERIOD);
        case '-': return make_token(lexer, TOK_MINUS);
        case '<':
            if (peek(lexer) == '-') {
                advance(lexer);
                return make_token(lexer, TOK_ATRIB);
            } else if (peek(lexer) == '>') {
                advance(lexer);
                return make_token(lexer, TOK_NEQ);
            } else if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(lexer, TOK_LTE);
            }
            return make_token(lexer, TOK_LT);
        case '>':
            if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(lexer, TOK_GTE);
            }
            return make_token(lexer, TOK_GT);
        case '=': return make_token(lexer, TOK_EQ);
        default:
            return make_token(lexer, TOK_ERROR);
    }
}

TokenList *lexer_tokenize(const char *input) {
    Lexer *lexer = lexer_create(input);
    TokenList *tokens = malloc(sizeof(TokenList));
    tokens->tokens = malloc(sizeof(Token) * 100);
    tokens->capacity = 100;
    tokens->count = 0;
    
    while (peek(lexer) != '\0') {
        skip_whitespace(lexer);
        if (peek(lexer) == '\0') break;
        
        Token token;
        if (isdigit(peek(lexer))) {
            token = scan_number(lexer);
        } else if (isalpha(peek(lexer)) || peek(lexer) == '_') {
            token = scan_identifier(lexer);
        } else {
            token = scan_operator(lexer);
        }
        
        if (tokens->count >= tokens->capacity) {
            tokens->capacity *= 2;
            tokens->tokens = realloc(tokens->tokens, sizeof(Token) * tokens->capacity);
        }
        
        tokens->tokens[tokens->count++] = token;
    }
    
    // Add EOF token
    Token eof_token;
    eof_token.type = TOK_EOF;
    eof_token.lexeme = strdup("");
    eof_token.lexeme_len = 0;
    eof_token.line = lexer->line;
    eof_token.column = lexer->column;
    eof_token.value.int_value = 0;
    
    if (tokens->count >= tokens->capacity) {
        tokens->capacity *= 2;
        tokens->tokens = realloc(tokens->tokens, sizeof(Token) * tokens->capacity);
    }
    
    tokens->tokens[tokens->count++] = eof_token;
    
    lexer_free(lexer);
    return tokens;
}
/* token_list_free and token_type_name are defined in token.c */
