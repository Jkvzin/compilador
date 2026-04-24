#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Helper function to duplicate strings */
static char *str_dup(const char *str) {
    if (!str) return NULL;
    char *dup = malloc(strlen(str) + 1);
    if (dup) {
        strcpy(dup, str);
    }
    return dup;
}

/* Helper function to create a new AST node */
static ASTNode *ast_alloc(ASTNodeType type) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node) {
        node->type = type;
        node->line = 0;
        node->column = 0;
        memset(&node->data, 0, sizeof(ASTNodeData));
    }
    return node;
}

/* Factory function: Create a program node */
ASTNode *ast_make_program(const char *name, ASTNode *decls, ASTNode *cmds) {
    ASTNode *node = ast_alloc(NODE_PROGRAM);
    if (node) {
        node->data.program.name = str_dup(name);
        node->data.program.declarations = decls;
        node->data.program.commands = cmds;
    }
    return node;
}

/* Factory function: Create a declaration node */
ASTNode *ast_make_declaration(DataType type, const char *name) {
    ASTNode *node = ast_alloc(NODE_DECLARATION);
    if (node) {
        node->data.declaration.type = type;
        node->data.declaration.name = str_dup(name);
    }
    return node;
}

/* Factory function: Create an empty declarations list */
ASTNode *ast_make_declarations(void) {
    ASTNode *node = ast_alloc(NODE_DECLARATIONS);
    if (node) {
        node->data.declarations.items = NULL;
        node->data.declarations.count = 0;
        node->data.declarations.capacity = 0;
    }
    return node;
}

/* Factory function: Create an empty commands list */
ASTNode *ast_make_commands(void) {
    ASTNode *node = ast_alloc(NODE_COMMANDS);
    if (node) {
        node->data.commands.items = NULL;
        node->data.commands.count = 0;
        node->data.commands.capacity = 0;
    }
    return node;
}

/* Factory function: Create an assignment node */
ASTNode *ast_make_assignment(const char *var, ASTNode *expr) {
    ASTNode *node = ast_alloc(NODE_ASSIGNMENT);
    if (node) {
        node->data.assignment.var_name = str_dup(var);
        node->data.assignment.expr = expr;
    }
    return node;
}

/* Factory function: Create a read node */
ASTNode *ast_make_read(const char *var) {
    ASTNode *node = ast_alloc(NODE_READ);
    if (node) {
        node->data.read.var_name = str_dup(var);
    }
    return node;
}

/* Factory function: Create a write node (write or writeln) */
ASTNode *ast_make_write(ASTNode *expr, int newline) {
    ASTNode *node = ast_alloc(newline ? NODE_WRITELN : NODE_WRITE);
    if (node) {
        node->data.write.expr = expr;
    }
    return node;
}

/* Factory function: Create an if statement node */
ASTNode *ast_make_if(ASTNode *cond, ASTNode *then_br, ASTNode *else_br) {
    ASTNode *node = ast_alloc(NODE_IF);
    if (node) {
        node->data.if_stmt.condition = cond;
        node->data.if_stmt.then_branch = then_br;
        node->data.if_stmt.else_branch = else_br;
    }
    return node;
}

/* Factory function: Create a while loop node */
ASTNode *ast_make_while(ASTNode *cond, ASTNode *body) {
    ASTNode *node = ast_alloc(NODE_WHILE);
    if (node) {
        node->data.while_stmt.condition = cond;
        node->data.while_stmt.body = body;
    }
    return node;
}

/* Factory function: Create a for loop node */
ASTNode *ast_make_for(const char *var, ASTNode *start, ASTNode *end, ASTNode *step, ASTNode *body) {
    ASTNode *node = ast_alloc(NODE_FOR);
    if (node) {
        node->data.for_stmt.var_name = str_dup(var);
        node->data.for_stmt.start_expr = start;
        node->data.for_stmt.end_expr = end;
        node->data.for_stmt.step_expr = step;
        node->data.for_stmt.body = body;
    }
    return node;
}

/* Factory function: Create a binary operation node */
ASTNode *ast_make_binop(ASTNode *left, char op, ASTNode *right) {
    ASTNode *node = ast_alloc(NODE_BINOP);
    if (node) {
        node->data.binop.left = left;
        node->data.binop.op = op;
        node->data.binop.right = right;
    }
    return node;
}

/* Factory function: Create a unary operation node */
ASTNode *ast_make_unop(char op, ASTNode *operand) {
    ASTNode *node = ast_alloc(NODE_UNOP);
    if (node) {
        node->data.unop.op = op;
        node->data.unop.operand = operand;
    }
    return node;
}

/* Factory function: Create an identifier node */
ASTNode *ast_make_id(const char *name) {
    ASTNode *node = ast_alloc(NODE_ID);
    if (node) {
        node->data.id.name = str_dup(name);
    }
    return node;
}

/* Factory function: Create an integer literal node */
ASTNode *ast_make_num_int(long long value) {
    ASTNode *node = ast_alloc(NODE_NUM_INT);
    if (node) {
        node->data.num_int.value = value;
    }
    return node;
}

/* Factory function: Create a float literal node */
ASTNode *ast_make_num_float(double value) {
    ASTNode *node = ast_alloc(NODE_NUM_FLOAT);
    if (node) {
        node->data.num_float.value = value;
    }
    return node;
}

/* Utility function: Add a child node to declarations or commands list */
void ast_add_child(ASTNode *parent, ASTNode *child) {
    if (!parent || !child) return;
    
    if (parent->type == NODE_DECLARATIONS) {
        /* Add to declarations list */
        if (parent->data.declarations.count >= parent->data.declarations.capacity) {
            size_t new_capacity = (parent->data.declarations.capacity == 0) ? 10 : parent->data.declarations.capacity * 2;
            ASTNode **new_items = realloc(parent->data.declarations.items, new_capacity * sizeof(ASTNode *));
            if (!new_items) return;
            parent->data.declarations.items = new_items;
            parent->data.declarations.capacity = new_capacity;
        }
        parent->data.declarations.items[parent->data.declarations.count++] = child;
    } else if (parent->type == NODE_COMMANDS) {
        /* Add to commands list */
        if (parent->data.commands.count >= parent->data.commands.capacity) {
            size_t new_capacity = (parent->data.commands.capacity == 0) ? 10 : parent->data.commands.capacity * 2;
            ASTNode **new_items = realloc(parent->data.commands.items, new_capacity * sizeof(ASTNode *));
            if (!new_items) return;
            parent->data.commands.items = new_items;
            parent->data.commands.capacity = new_capacity;
        }
        parent->data.commands.items[parent->data.commands.count++] = child;
    }
}

/* Utility function: Get the name of an AST node type for debugging */
static const char *ast_node_type_name(ASTNodeType type) {
    switch (type) {
        case NODE_PROGRAM: return "PROGRAM";
        case NODE_DECLARATION: return "DECLARATION";
        case NODE_DECLARATIONS: return "DECLARATIONS";
        case NODE_COMMANDS: return "COMMANDS";
        case NODE_ASSIGNMENT: return "ASSIGNMENT";
        case NODE_READ: return "READ";
        case NODE_WRITE: return "WRITE";
        case NODE_WRITELN: return "WRITELN";
        case NODE_IF: return "IF";
        case NODE_WHILE: return "WHILE";
        case NODE_FOR: return "FOR";
        case NODE_BINOP: return "BINOP";
        case NODE_UNOP: return "UNOP";
        case NODE_ID: return "ID";
        case NODE_NUM_INT: return "NUM_INT";
        case NODE_NUM_FLOAT: return "NUM_FLOAT";
        default: return "UNKNOWN";
    }
}

/* Utility function: Get the name of a data type for debugging */
static const char *data_type_name(DataType type) {
    switch (type) {
        case TYPE_INT: return "int";
        case TYPE_FLOAT: return "float";
        case TYPE_VOID: return "void";
        case TYPE_UNKNOWN: return "unknown";
        default: return "?";
    }
}

/* Utility function: Print AST for debugging */
void ast_print(ASTNode *node, int depth) {
    if (!node) return;
    
    const char *indent = "  ";
    for (int i = 0; i < depth; i++) {
        printf("%s", indent);
    }
    
    printf("[%s", ast_node_type_name(node->type));
    if (node->line > 0 || node->column > 0) {
        printf(" @ line %d col %d", node->line, node->column);
    }
    printf("]\n");
    
    /* Print node-specific data */
    switch (node->type) {
        case NODE_PROGRAM:
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("name: %s\n", node->data.program.name ? node->data.program.name : "?");
            if (node->data.program.declarations) {
                ast_print(node->data.program.declarations, depth + 1);
            }
            if (node->data.program.commands) {
                ast_print(node->data.program.commands, depth + 1);
            }
            break;
            
        case NODE_DECLARATION:
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("type: %s, name: %s\n", 
                   data_type_name(node->data.declaration.type),
                   node->data.declaration.name ? node->data.declaration.name : "?");
            break;
            
        case NODE_DECLARATIONS:
            for (size_t i = 0; i < node->data.declarations.count; i++) {
                ast_print(node->data.declarations.items[i], depth + 1);
            }
            break;
            
        case NODE_COMMANDS:
            for (size_t i = 0; i < node->data.commands.count; i++) {
                ast_print(node->data.commands.items[i], depth + 1);
            }
            break;
            
        case NODE_ASSIGNMENT:
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("var: %s\n", node->data.assignment.var_name ? node->data.assignment.var_name : "?");
            if (node->data.assignment.expr) {
                ast_print(node->data.assignment.expr, depth + 1);
            }
            break;
            
        case NODE_READ:
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("var: %s\n", node->data.read.var_name ? node->data.read.var_name : "?");
            break;
            
        case NODE_WRITE:
        case NODE_WRITELN:
            if (node->data.write.expr) {
                ast_print(node->data.write.expr, depth + 1);
            }
            break;
            
        case NODE_IF:
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("condition:\n");
            if (node->data.if_stmt.condition) {
                ast_print(node->data.if_stmt.condition, depth + 1);
            }
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("then:\n");
            if (node->data.if_stmt.then_branch) {
                ast_print(node->data.if_stmt.then_branch, depth + 1);
            }
            if (node->data.if_stmt.else_branch) {
                for (int i = 0; i <= depth; i++) printf("%s", indent);
                printf("else:\n");
                ast_print(node->data.if_stmt.else_branch, depth + 1);
            }
            break;
            
        case NODE_WHILE:
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("condition:\n");
            if (node->data.while_stmt.condition) {
                ast_print(node->data.while_stmt.condition, depth + 1);
            }
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("body:\n");
            if (node->data.while_stmt.body) {
                ast_print(node->data.while_stmt.body, depth + 1);
            }
            break;
            
        case NODE_FOR:
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("var: %s\n", node->data.for_stmt.var_name ? node->data.for_stmt.var_name : "?");
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("start:\n");
            if (node->data.for_stmt.start_expr) {
                ast_print(node->data.for_stmt.start_expr, depth + 1);
            }
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("end:\n");
            if (node->data.for_stmt.end_expr) {
                ast_print(node->data.for_stmt.end_expr, depth + 1);
            }
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("step:\n");
            if (node->data.for_stmt.step_expr) {
                ast_print(node->data.for_stmt.step_expr, depth + 1);
            }
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("body:\n");
            if (node->data.for_stmt.body) {
                ast_print(node->data.for_stmt.body, depth + 1);
            }
            break;
            
        case NODE_BINOP:
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("op: '%c'\n", node->data.binop.op);
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("left:\n");
            if (node->data.binop.left) {
                ast_print(node->data.binop.left, depth + 1);
            }
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("right:\n");
            if (node->data.binop.right) {
                ast_print(node->data.binop.right, depth + 1);
            }
            break;
            
        case NODE_UNOP:
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("op: '%c'\n", node->data.unop.op);
            if (node->data.unop.operand) {
                ast_print(node->data.unop.operand, depth + 1);
            }
            break;
            
        case NODE_ID:
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("name: %s\n", node->data.id.name ? node->data.id.name : "?");
            break;
            
        case NODE_NUM_INT:
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("value: %lld\n", node->data.num_int.value);
            break;
            
        case NODE_NUM_FLOAT:
            for (int i = 0; i <= depth; i++) printf("%s", indent);
            printf("value: %f\n", node->data.num_float.value);
            break;
    }
}

/* Utility function: Recursively free an AST node and its children */
void ast_free(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_PROGRAM:
            free(node->data.program.name);
            ast_free(node->data.program.declarations);
            ast_free(node->data.program.commands);
            break;
            
        case NODE_DECLARATION:
            free(node->data.declaration.name);
            break;
            
        case NODE_DECLARATIONS:
            for (size_t i = 0; i < node->data.declarations.count; i++) {
                ast_free(node->data.declarations.items[i]);
            }
            free(node->data.declarations.items);
            break;
            
        case NODE_COMMANDS:
            for (size_t i = 0; i < node->data.commands.count; i++) {
                ast_free(node->data.commands.items[i]);
            }
            free(node->data.commands.items);
            break;
            
        case NODE_ASSIGNMENT:
            free(node->data.assignment.var_name);
            ast_free(node->data.assignment.expr);
            break;
            
        case NODE_READ:
            free(node->data.read.var_name);
            break;
            
        case NODE_WRITE:
        case NODE_WRITELN:
            ast_free(node->data.write.expr);
            break;
            
        case NODE_IF:
            ast_free(node->data.if_stmt.condition);
            ast_free(node->data.if_stmt.then_branch);
            ast_free(node->data.if_stmt.else_branch);
            break;
            
        case NODE_WHILE:
            ast_free(node->data.while_stmt.condition);
            ast_free(node->data.while_stmt.body);
            break;
            
        case NODE_FOR:
            free(node->data.for_stmt.var_name);
            ast_free(node->data.for_stmt.start_expr);
            ast_free(node->data.for_stmt.end_expr);
            ast_free(node->data.for_stmt.step_expr);
            ast_free(node->data.for_stmt.body);
            break;
            
        case NODE_BINOP:
            ast_free(node->data.binop.left);
            ast_free(node->data.binop.right);
            break;
            
        case NODE_UNOP:
            ast_free(node->data.unop.operand);
            break;
            
        case NODE_ID:
            free(node->data.id.name);
            break;
            
        case NODE_NUM_INT:
        case NODE_NUM_FLOAT:
            /* No dynamic allocation for literals */
            break;
    }
    
    free(node);
}
