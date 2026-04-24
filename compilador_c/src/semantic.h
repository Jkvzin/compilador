#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "symbol.h"

/* Semantic analyzer result */
typedef struct {
    int is_valid;           /* 1 if valid, 0 if errors */
    char error_message[256]; /* Error message if invalid */
    int error_line;
    int error_column;
} SemanticResult;

/* Semantic analyzer structure */
typedef struct {
    SymbolTable *symbols;
    int error_count;
    char last_error[256];
} SemanticAnalyzer;

/* Create semantic analyzer */
SemanticAnalyzer *semantic_create(void);

/* Free semantic analyzer */
void semantic_free(SemanticAnalyzer *analyzer);

/* Analyze complete program */
SemanticResult semantic_analyze_program(SemanticAnalyzer *analyzer, ASTNode *program);

/* Helper functions */
int semantic_check_declaration(SemanticAnalyzer *analyzer, ASTNode *decl);
int semantic_check_statement(SemanticAnalyzer *analyzer, ASTNode *stmt);
int semantic_check_expression(SemanticAnalyzer *analyzer, ASTNode *expr);
int semantic_check_binop(SemanticAnalyzer *analyzer, ASTNode *binop);
int semantic_check_unop(SemanticAnalyzer *analyzer, ASTNode *unop);
int semantic_get_type(SemanticAnalyzer *analyzer, ASTNode *node);

#endif /* SEMANTIC_H */
