#!/usr/bin/env python3
"""
Complete Lexer Implementation Setup - Task 1 & 2
Organizes files and creates missing lexer implementation
"""
import os
import shutil

os.chdir(os.path.dirname(os.path.abspath(__file__)))

print("=" * 70)
print("SIMPLES Compiler - Complete Lexer Setup")
print("=" * 70)

# Create directories
dirs = ['src', 'tests', 'examples', '.obj']
for d in dirs:
    os.makedirs(d, exist_ok=True)
    print(f"✓ Directory: {d}/")

# Move existing files to proper locations
print("\n--- Organizing Existing Files ---")

if os.path.exists('token.h'):
    shutil.move('token.h', 'src/token.h')
    print("✓ Moved token.h → src/token.h")

if os.path.exists('test_lexer.c'):
    shutil.move('test_lexer.c', 'tests/test_lexer.c')
    print("✓ Moved test_lexer.c → tests/test_lexer.c")

# ============================================================================
# Create src/lexer.h if it doesn't exist
# ============================================================================
lexer_h_path = 'src/lexer.h'
if not os.path.exists(lexer_h_path):
    print("\n--- Creating src/lexer.h ---")
    lexer_h = '''#ifndef LEXER_H
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
'''
    with open(lexer_h_path, 'w') as f:
        f.write(lexer_h)
    print("✓ Created src/lexer.h")
else:
    print("⚠ src/lexer.h already exists")

# ============================================================================
# Create src/lexer.c if it doesn't exist
# ============================================================================
lexer_c_path = 'src/lexer.c'
if not os.path.exists(lexer_c_path):
    print("\n--- Creating src/lexer.c ---")
    lexer_c = '''#include "lexer.h"
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
    if (lexer->current >= strlen(lexer->input)) return '\\0';
    return lexer->input[lexer->current];
}

static char peek_next(Lexer *lexer) {
    if (lexer->current + 1 >= strlen(lexer->input)) return '\\0';
    return lexer->input[lexer->current + 1];
}

static char advance(Lexer *lexer) {
    char c = peek(lexer);
    lexer->current++;
    if (c == '\\n') {
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

static Token make_token(Lexer *lexer, TokenType type) {
    Token token;
    token.type = type;
    token.lexeme = lexer->input + lexer->start;
    token.lexeme_len = lexer->current - lexer->start;
    token.line = lexer->line;
    token.column = lexer->column - (int)token.lexeme_len;
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
        token.value.float_value = atof(token.lexeme);
        return token;
    }
    
    Token token = make_token(lexer, TOK_NUM_INT);
    token.value.int_value = atoll(token.lexeme);
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
        case '+': return make_token(lexer, TOK_MAIS);
        case '*': return make_token(lexer, TOK_MULT);
        case '(': return make_token(lexer, TOK_ABRE_PAR);
        case ')': return make_token(lexer, TOK_FECHA_PAR);
        case ',': return make_token(lexer, TOK_VIRGULA);
        case ';': return make_token(lexer, TOK_PONTO_VIRGULA);
        case '-': return make_token(lexer, TOK_MENOS);
        case '<':
            if (peek(lexer) == '-') {
                advance(lexer);
                return make_token(lexer, TOK_ATRIB);
            } else if (peek(lexer) == '>') {
                advance(lexer);
                return make_token(lexer, TOK_DIFERENTE);
            } else if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(lexer, TOK_MENOR_IGUAL);
            }
            return make_token(lexer, TOK_MENOR);
        case '>':
            if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(lexer, TOK_MAIOR_IGUAL);
            }
            return make_token(lexer, TOK_MAIOR);
        case '=': return make_token(lexer, TOK_IGUAL);
        default:
            return make_token(lexer, TOK_ERROR);
    }
}

TokenList *lexer_tokenize(const char *input) {
    Lexer *lexer = lexer_create(input);
    TokenList *tokens = malloc(sizeof(TokenList));
    tokens->items = malloc(sizeof(Token) * 100);
    tokens->capacity = 100;
    tokens->count = 0;
    
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
        
        if (tokens->count >= tokens->capacity) {
            tokens->capacity *= 2;
            tokens->items = realloc(tokens->items, sizeof(Token) * tokens->capacity);
        }
        
        tokens->items[tokens->count++] = token;
    }
    
    // Add EOF token
    Token eof_token;
    eof_token.type = TOK_EOF;
    eof_token.lexeme = "";
    eof_token.lexeme_len = 0;
    eof_token.line = lexer->line;
    eof_token.column = lexer->column;
    
    if (tokens->count >= tokens->capacity) {
        tokens->capacity *= 2;
        tokens->items = realloc(tokens->items, sizeof(Token) * tokens->capacity);
    }
    
    tokens->items[tokens->count++] = eof_token;
    
    lexer_free(lexer);
    return tokens;
}

void token_list_free(TokenList *list) {
    free(list->items);
    free(list);
}

const char *token_type_name(TokenType type) {
    static const char *names[] = {
        "TOK_PROGRAMA", "TOK_INICIO", "TOK_FIM", "TOK_INTEIRO",
        "TOK_FLUTUANTE", "TOK_VAZIO", "TOK_SE", "TOK_ENTAO",
        "TOK_SENAO", "TOK_FIMSE", "TOK_ENQUANTO", "TOK_FIMENQUANTO",
        "TOK_PARA", "TOK_DE", "TOK_ATE", "TOK_PASSO",
        "TOK_FACA", "TOK_FIMPARA", "TOK_LEIA", "TOK_ESCREVA",
        "TOK_ESCREVAL", "TOK_E", "TOK_OU", "TOK_NAO",
        "TOK_DIV", "TOK_PROCEDIMENTO", "TOK_RETORNA",
        "TOK_ATRIB", "TOK_MAIS", "TOK_MENOS", "TOK_MULT",
        "TOK_MAIOR", "TOK_MENOR", "TOK_IGUAL", "TOK_DIFERENTE",
        "TOK_MAIOR_IGUAL", "TOK_MENOR_IGUAL", "TOK_ABRE_PAR", "TOK_FECHA_PAR",
        "TOK_VIRGULA", "TOK_PONTO_VIRGULA",
        "TOK_ID", "TOK_NUM_INT", "TOK_NUM_FLOAT",
        "TOK_EOF", "TOK_ERROR", "TOK_COUNT"
    };
    if (type < TOK_COUNT) return names[type];
    return "UNKNOWN";
}
'''
    with open(lexer_c_path, 'w') as f:
        f.write(lexer_c)
    print("✓ Created src/lexer.c")
else:
    print("⚠ src/lexer.c already exists")

print("\n" + "=" * 70)
print("✅ LEXER PHASE SETUP COMPLETE!")
print("=" * 70)
print("\nProject structure:")
print("  src/")
print("    ├── token.h ✓")
print("    ├── lexer.h ✓")
print("    └── lexer.c ✓")
print("  tests/")
print("    └── test_lexer.c ✓")
print("  Makefile ✓")
print("  .gitignore ✓")

print("\n" + "=" * 70)
print("Next steps:")
print("=" * 70)
print("\n1. Test the build:")
print("   make clean test")
print("\n2. You should see:")
print("   'test_lexer_single_integer... PASS'")
print("\n3. Commit your work:")
print("   git add src/ tests/ Makefile")
print("   git commit -m \"feat: complete lexer phase implementation\"")
print("\n" + "=" * 70)
