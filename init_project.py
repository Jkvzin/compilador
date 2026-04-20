#!/usr/bin/env python3
"""
SIMPLES Compiler - Complete Project Initialization
Sets up all files needed for Tasks 1-3 (Lexer Phase)
"""
import os
import subprocess

script_dir = os.path.dirname(os.path.abspath(__file__))
os.chdir(script_dir)

# Create directories
dirs = ['src', 'tests', 'examples', '.obj']
for d in dirs:
    os.makedirs(d, exist_ok=True)

print("=" * 60)
print("SIMPLES Compiler - Project Initialization")
print("=" * 60)

# ============================================================================
# Create src/token.h
# ============================================================================
token_h_content = '''#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

typedef enum {
    // Keywords (27 total)
    TOK_PROGRAMA = 0,
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
    
    // Operators & Delimiters (16 total)
    TOK_ATRIB,          // <-
    TOK_MAIS,           // +
    TOK_MENOS,          // -
    TOK_MULT,           // *
    TOK_MAIOR,          // >
    TOK_MENOR,          // <
    TOK_IGUAL,          // =
    TOK_DIFERENTE,      // <>
    TOK_MAIOR_IGUAL,    // >=
    TOK_MENOR_IGUAL,    // <=
    TOK_ABRE_PAR,       // (
    TOK_FECHA_PAR,      // )
    TOK_VIRGULA,        // ,
    TOK_PONTO_VIRGULA,  // ;
    
    // Literals & Identifiers (3 total)
    TOK_ID,
    TOK_NUM_INT,
    TOK_NUM_FLOAT,
    
    // Special (2 total)
    TOK_EOF,
    TOK_ERROR,
    
    TOK_COUNT
} TokenType;

typedef struct {
    TokenType type;
    const char *lexeme;
    size_t lexeme_len;
    int line;
    int column;
    union {
        long long int_value;
        double float_value;
    } value;
} Token;

typedef struct {
    Token *items;
    size_t count;
    size_t capacity;
} TokenList;

// Utility
const char *token_type_name(TokenType type);

#endif
'''

with open('src/token.h', 'w') as f:
    f.write(token_h_content)
print("✓ Created src/token.h")

# ============================================================================
# Create Makefile
# ============================================================================
makefile_content = '''CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -g -fPIC
SRCDIR  = src
TESTDIR = tests
OBJDIR  = .obj
BINDIR  = .

# Source files
SOURCES  = $(wildcard $(SRCDIR)/*.c)
HEADERS  = $(wildcard $(SRCDIR)/*.h)
OBJECTS  = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Exclude main.o from test builds
TEST_OBJECTS = $(filter-out $(OBJDIR)/main.o, $(OBJECTS))

# Test files
TEST_SOURCES = $(wildcard $(TESTDIR)/test_*.c)
TEST_BIN = $(BINDIR)/run_tests

COMPILER_BIN = $(BINDIR)/simplesc

.PHONY: all test clean help

all: $(COMPILER_BIN)

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(COMPILER_BIN): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_BIN): $(TEST_SOURCES) $(TEST_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ -Isrc

test: $(TEST_BIN)
	@$(TEST_BIN)

clean:
	@rm -rf $(OBJDIR) $(COMPILER_BIN) $(TEST_BIN)
	@echo "Cleaned"

help:
	@echo "Targets: all, test, clean"
'''

with open('Makefile', 'w') as f:
    f.write(makefile_content)
print("✓ Created Makefile")

# ============================================================================
# Create src/lexer.h
# ============================================================================
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

// Lexer functions
Lexer *lexer_create(const char *input);
void lexer_free(Lexer *lexer);

// Tokenization
TokenList *lexer_tokenize(const char *input);
void token_list_free(TokenList *list);

#endif
'''

with open('src/lexer.h', 'w') as f:
    f.write(lexer_h_content)
print("✓ Created src/lexer.h")

# ============================================================================
# Create src/lexer.c (full implementation with all token scanning)
# ============================================================================
lexer_c_content = '''#include "lexer.h"
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

with open('src/lexer.c', 'w') as f:
    f.write(lexer_c_content)
print("✓ Created src/lexer.c")

# ============================================================================
# Create tests/test_lexer.c (comprehensive test suite)
# ============================================================================
test_lexer_content = '''#include <stdio.h>
#include <string.h>
#include "../src/lexer.h"

// Simple assertions for testing
#define ASSERT_EQUAL(a, b) \\
    if ((a) != (b)) { \\
        printf("FAIL: %s:%d - Expected %d, got %d\\n", __FILE__, __LINE__, (int)(b), (int)(a)); \\
        return 0; \\
    }

#define ASSERT_STRING_EQUAL(a, b) \\
    if (strcmp((a), (b)) != 0) { \\
        printf("FAIL: %s:%d - Expected '%s', got '%s'\\n", __FILE__, __LINE__, (b), (a)); \\
        return 0; \\
    }

#define ASSERT_NOT_NULL(a) \\
    if ((a) == NULL) { \\
        printf("FAIL: %s:%d - Expected non-NULL\\n", __FILE__, __LINE__); \\
        return 0; \\
    }

int test_lexer_single_integer(void) {
    TokenList *tokens = lexer_tokenize("42");
    ASSERT_NOT_NULL(tokens);
    ASSERT_EQUAL(tokens->count, 2);  // 42 + EOF
    ASSERT_EQUAL(tokens->items[0].type, TOK_NUM_INT);
    ASSERT_EQUAL(tokens->items[0].value.int_value, 42);
    ASSERT_EQUAL(tokens->items[1].type, TOK_EOF);
    token_list_free(tokens);
    return 1;
}

int test_lexer_multiple_integers(void) {
    TokenList *tokens = lexer_tokenize("42 123 0");
    ASSERT_EQUAL(tokens->count, 4);  // 42, 123, 0, EOF
    ASSERT_EQUAL(tokens->items[0].type, TOK_NUM_INT);
    ASSERT_EQUAL(tokens->items[0].value.int_value, 42);
    ASSERT_EQUAL(tokens->items[1].type, TOK_NUM_INT);
    ASSERT_EQUAL(tokens->items[1].value.int_value, 123);
    ASSERT_EQUAL(tokens->items[2].type, TOK_NUM_INT);
    ASSERT_EQUAL(tokens->items[2].value.int_value, 0);
    token_list_free(tokens);
    return 1;
}

int test_lexer_operators(void) {
    TokenList *tokens = lexer_tokenize("<- + - * > < = <> >= <=");
    ASSERT_EQUAL(tokens->items[0].type, TOK_ATRIB);
    ASSERT_EQUAL(tokens->items[1].type, TOK_MAIS);
    ASSERT_EQUAL(tokens->items[2].type, TOK_MENOS);
    ASSERT_EQUAL(tokens->items[3].type, TOK_MULT);
    ASSERT_EQUAL(tokens->items[4].type, TOK_MAIOR);
    ASSERT_EQUAL(tokens->items[5].type, TOK_MENOR);
    ASSERT_EQUAL(tokens->items[6].type, TOK_IGUAL);
    ASSERT_EQUAL(tokens->items[7].type, TOK_DIFERENTE);
    ASSERT_EQUAL(tokens->items[8].type, TOK_MAIOR_IGUAL);
    ASSERT_EQUAL(tokens->items[9].type, TOK_MENOR_IGUAL);
    token_list_free(tokens);
    return 1;
}

int test_lexer_keywords(void) {
    TokenList *tokens = lexer_tokenize("programa inteiro se entao senao fimse enquanto para");
    ASSERT_EQUAL(tokens->items[0].type, TOK_PROGRAMA);
    ASSERT_EQUAL(tokens->items[1].type, TOK_INTEIRO);
    ASSERT_EQUAL(tokens->items[2].type, TOK_SE);
    ASSERT_EQUAL(tokens->items[3].type, TOK_ENTAO);
    ASSERT_EQUAL(tokens->items[4].type, TOK_SENAO);
    ASSERT_EQUAL(tokens->items[5].type, TOK_FIMSE);
    ASSERT_EQUAL(tokens->items[6].type, TOK_ENQUANTO);
    ASSERT_EQUAL(tokens->items[7].type, TOK_PARA);
    token_list_free(tokens);
    return 1;
}

int test_lexer_delimiters(void) {
    TokenList *tokens = lexer_tokenize("( ) , ;");
    ASSERT_EQUAL(tokens->items[0].type, TOK_ABRE_PAR);
    ASSERT_EQUAL(tokens->items[1].type, TOK_FECHA_PAR);
    ASSERT_EQUAL(tokens->items[2].type, TOK_VIRGULA);
    ASSERT_EQUAL(tokens->items[3].type, TOK_PONTO_VIRGULA);
    token_list_free(tokens);
    return 1;
}

int test_lexer_identifiers(void) {
    TokenList *tokens = lexer_tokenize("x variavel _interno var123");
    ASSERT_EQUAL(tokens->items[0].type, TOK_ID);
    ASSERT_EQUAL(tokens->items[1].type, TOK_ID);
    ASSERT_EQUAL(tokens->items[2].type, TOK_ID);
    ASSERT_EQUAL(tokens->items[3].type, TOK_ID);
    token_list_free(tokens);
    return 1;
}

int test_lexer_program_example(void) {
    const char *input = "programa teste\\n  inteiro x\\ninicio\\n  x <- 42\\nfim";
    TokenList *tokens = lexer_tokenize(input);
    ASSERT_EQUAL(tokens->items[0].type, TOK_PROGRAMA);
    ASSERT_EQUAL(tokens->items[1].type, TOK_ID);  // teste
    ASSERT_EQUAL(tokens->items[2].type, TOK_INTEIRO);
    ASSERT_EQUAL(tokens->items[3].type, TOK_ID);  // x
    ASSERT_EQUAL(tokens->items[4].type, TOK_INICIO);
    ASSERT_EQUAL(tokens->items[5].type, TOK_ID);  // x
    ASSERT_EQUAL(tokens->items[6].type, TOK_ATRIB);
    ASSERT_EQUAL(tokens->items[7].type, TOK_NUM_INT);
    ASSERT_EQUAL(tokens->items[8].type, TOK_FIM);
    token_list_free(tokens);
    return 1;
}

int main(void) {
    int passed = 0, failed = 0;
    
    printf("Running Lexer Tests...\\n");
    printf("======================\\n");
    
    #define RUN_TEST(test) \\
        printf("Running %s... ", #test); \\
        if (test()) { printf("PASS\\n"); passed++; } else { failed++; }
    
    RUN_TEST(test_lexer_single_integer);
    RUN_TEST(test_lexer_multiple_integers);
    RUN_TEST(test_lexer_operators);
    RUN_TEST(test_lexer_keywords);
    RUN_TEST(test_lexer_delimiters);
    RUN_TEST(test_lexer_identifiers);
    RUN_TEST(test_lexer_program_example);
    
    printf("======================\\n");
    printf("Results: %d passed, %d failed\\n", passed, failed);
    
    return failed == 0 ? 0 : 1;
}
'''

with open('tests/test_lexer.c', 'w') as f:
    f.write(test_lexer_content)
print("✓ Created tests/test_lexer.c")

print("\\n" + "=" * 60)
print("Project structure created successfully!")
print("\\nNext steps:")
print("1. cd to the project directory")
print("2. Run: make test")
print("   Expected: 7 tests pass, 0 fail")
print("=" * 60)
'''

with open('init_project.py', 'w') as f:
    f.write(init_script)

print("✓ Created init_project.py")

if __name__ == '__main__':
    print("\nProject setup complete! All source files created.")
    print("You can now run: make test")
