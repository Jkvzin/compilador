#ifndef AST_H
#define AST_H

#include <stddef.h>

/* Data types in SIMPLES */
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_VOID,
    TYPE_UNKNOWN
} DataType;

/* AST Node types for all language constructs */
typedef enum {
    NODE_PROGRAM,
    NODE_DECLARATION,
    NODE_DECLARATIONS,
    NODE_COMMANDS,
    NODE_ASSIGNMENT,
    NODE_READ,
    NODE_WRITE,
    NODE_WRITELN,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_BINOP,
    NODE_UNOP,
    NODE_ID,
    NODE_NUM_INT,
    NODE_NUM_FLOAT
} ASTNodeType;

/* Forward declaration */
typedef struct ASTNode ASTNode;

/* Union containing node-specific data */
typedef union {
    /* NODE_PROGRAM */
    struct {
        char *name;
        ASTNode *declarations;
        ASTNode *commands;
    } program;
    
    /* NODE_DECLARATION */
    struct {
        DataType type;
        char *name;
    } declaration;
    
    /* NODE_DECLARATIONS - list of declarations */
    struct {
        ASTNode **items;
        size_t count;
        size_t capacity;
    } declarations;
    
    /* NODE_COMMANDS - list of commands */
    struct {
        ASTNode **items;
        size_t count;
        size_t capacity;
    } commands;
    
    /* NODE_ASSIGNMENT */
    struct {
        char *var_name;
        ASTNode *expr;
    } assignment;
    
    /* NODE_READ */
    struct {
        char *var_name;
    } read;
    
    /* NODE_WRITE / NODE_WRITELN */
    struct {
        ASTNode *expr;
    } write;
    
    /* NODE_IF */
    struct {
        ASTNode *condition;
        ASTNode *then_branch;
        ASTNode *else_branch;
    } if_stmt;
    
    /* NODE_WHILE */
    struct {
        ASTNode *condition;
        ASTNode *body;
    } while_stmt;
    
    /* NODE_FOR */
    struct {
        char *var_name;
        ASTNode *start_expr;
        ASTNode *end_expr;
        ASTNode *step_expr;
        ASTNode *body;
    } for_stmt;
    
    /* NODE_BINOP */
    struct {
        ASTNode *left;
        char op;
        ASTNode *right;
    } binop;
    
    /* NODE_UNOP */
    struct {
        char op;
        ASTNode *operand;
    } unop;
    
    /* NODE_ID */
    struct {
        char *name;
    } id;
    
    /* NODE_NUM_INT */
    struct {
        long long value;
    } num_int;
    
    /* NODE_NUM_FLOAT */
    struct {
        double value;
    } num_float;
} ASTNodeData;

/* AST Node structure */
typedef struct ASTNode {
    ASTNodeType type;
    int line;
    int column;
    ASTNodeData data;
} ASTNode;

/* Factory functions for creating AST nodes */

ASTNode *ast_make_program(const char *name, ASTNode *decls, ASTNode *cmds);
ASTNode *ast_make_declaration(DataType type, const char *name);
ASTNode *ast_make_declarations(void);
ASTNode *ast_make_commands(void);
ASTNode *ast_make_assignment(const char *var, ASTNode *expr);
ASTNode *ast_make_read(const char *var);
ASTNode *ast_make_write(ASTNode *expr, int newline);
ASTNode *ast_make_if(ASTNode *cond, ASTNode *then_br, ASTNode *else_br);
ASTNode *ast_make_while(ASTNode *cond, ASTNode *body);
ASTNode *ast_make_for(const char *var, ASTNode *start, ASTNode *end, ASTNode *step, ASTNode *body);
ASTNode *ast_make_binop(ASTNode *left, char op, ASTNode *right);
ASTNode *ast_make_unop(char op, ASTNode *operand);
ASTNode *ast_make_id(const char *name);
ASTNode *ast_make_num_int(long long value);
ASTNode *ast_make_num_float(double value);

/* Utility functions */

void ast_add_child(ASTNode *parent, ASTNode *child);
void ast_free(ASTNode *node);
void ast_print(ASTNode *node, int depth);

#endif /* AST_H */
