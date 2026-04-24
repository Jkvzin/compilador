#include "semantic.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Create semantic analyzer */
SemanticAnalyzer *semantic_create(void) {
    SemanticAnalyzer *analyzer = malloc(sizeof(SemanticAnalyzer));
    if (!analyzer) return NULL;
    
    analyzer->symbols = symbol_table_create();
    if (!analyzer->symbols) {
        free(analyzer);
        return NULL;
    }
    
    analyzer->error_count = 0;
    strcpy(analyzer->last_error, "");
    
    return analyzer;
}

/* Free semantic analyzer */
void semantic_free(SemanticAnalyzer *analyzer) {
    if (analyzer) {
        symbol_table_free(analyzer->symbols);
        free(analyzer);
    }
}

/* Check declaration */
int semantic_check_declaration(SemanticAnalyzer *analyzer, ASTNode *decl) {
    if (!decl || decl->type != NODE_DECLARATION) return 0;
    
    const char *name = decl->data.declaration.name;
    DataType type = decl->data.declaration.type;
    int line = decl->line;
    int column = decl->column;
    
    /* Add to symbol table */
    if (symbol_table_add(analyzer->symbols, name, type, line, column) == -1) {
        sprintf(analyzer->last_error, "Duplicate declaration: %s", name);
        analyzer->error_count++;
        return 0;
    }
    
    return 1;
}

/* Check expression for undefined variables */
int semantic_check_expression(SemanticAnalyzer *analyzer, ASTNode *expr) {
    if (!expr) return 1;
    
    switch (expr->type) {
        case NODE_NUM_INT:
        case NODE_NUM_FLOAT:
            return 1;
        
        case NODE_ID: {
            const char *name = expr->data.id.name;
            if (!symbol_table_exists(analyzer->symbols, name)) {
                sprintf(analyzer->last_error, "Undefined variable: %s", name);
                analyzer->error_count++;
                return 0;
            }
            return 1;
        }
        
        case NODE_BINOP:
            return semantic_check_binop(analyzer, expr);
        
        case NODE_UNOP:
            return semantic_check_unop(analyzer, expr);
        
        default:
            return 1;
    }
}

/* Check binary operation */
int semantic_check_binop(SemanticAnalyzer *analyzer, ASTNode *binop) {
    if (!binop || binop->type != NODE_BINOP) return 0;
    
    /* Check both operands */
    if (!semantic_check_expression(analyzer, binop->data.binop.left)) return 0;
    if (!semantic_check_expression(analyzer, binop->data.binop.right)) return 0;
    
    /* For now, allow all operators on all types */
    /* Future: add type checking for operator compatibility */
    return 1;
}

/* Check unary operation */
int semantic_check_unop(SemanticAnalyzer *analyzer, ASTNode *unop) {
    if (!unop || unop->type != NODE_UNOP) return 0;
    
    return semantic_check_expression(analyzer, unop->data.unop.operand);
}

/* Check statement */
int semantic_check_statement(SemanticAnalyzer *analyzer, ASTNode *stmt) {
    if (!stmt) return 1;
    
    switch (stmt->type) {
        case NODE_ASSIGNMENT: {
            /* Check variable exists */
            const char *var = stmt->data.assignment.var_name;
            if (!symbol_table_exists(analyzer->symbols, var)) {
                sprintf(analyzer->last_error, "Undefined variable: %s", var);
                analyzer->error_count++;
                return 0;
            }
            /* Check expression */
            return semantic_check_expression(analyzer, stmt->data.assignment.expr);
        }
        
        case NODE_READ: {
            /* Check variable exists */
            const char *var = stmt->data.read.var_name;
            if (!symbol_table_exists(analyzer->symbols, var)) {
                sprintf(analyzer->last_error, "Undefined variable: %s", var);
                analyzer->error_count++;
                return 0;
            }
            return 1;
        }
        
        case NODE_WRITE:
        case NODE_WRITELN:
            return semantic_check_expression(analyzer, stmt->data.write.expr);
        
        case NODE_IF:
            if (!semantic_check_expression(analyzer, stmt->data.if_stmt.condition)) return 0;
            if (stmt->data.if_stmt.then_branch && !semantic_check_statement(analyzer, stmt->data.if_stmt.then_branch)) return 0;
            if (stmt->data.if_stmt.else_branch && !semantic_check_statement(analyzer, stmt->data.if_stmt.else_branch)) return 0;
            return 1;
        
        case NODE_WHILE:
            if (!semantic_check_expression(analyzer, stmt->data.while_stmt.condition)) return 0;
            return semantic_check_statement(analyzer, stmt->data.while_stmt.body);
        
        case NODE_FOR: {
            const char *var = stmt->data.for_stmt.var_name;
            if (!symbol_table_exists(analyzer->symbols, var)) {
                sprintf(analyzer->last_error, "Undefined loop variable: %s", var);
                analyzer->error_count++;
                return 0;
            }
            if (!semantic_check_expression(analyzer, stmt->data.for_stmt.start_expr)) return 0;
            if (!semantic_check_expression(analyzer, stmt->data.for_stmt.end_expr)) return 0;
            if (!semantic_check_expression(analyzer, stmt->data.for_stmt.step_expr)) return 0;
            return semantic_check_statement(analyzer, stmt->data.for_stmt.body);
        }
        
        case NODE_COMMANDS:
            for (size_t i = 0; i < stmt->data.commands.count; i++) {
                if (!semantic_check_statement(analyzer, stmt->data.commands.items[i])) return 0;
            }
            return 1;
        
        default:
            return 1;
    }
}

/* Analyze complete program */
SemanticResult semantic_analyze_program(SemanticAnalyzer *analyzer, ASTNode *program) {
    SemanticResult result = {1, "", 0, 0};
    
    if (!analyzer || !program || program->type != NODE_PROGRAM) {
        result.is_valid = 0;
        strcpy(result.error_message, "Invalid program node");
        return result;
    }
    
    /* Process declarations */
    ASTNode *decls = program->data.program.declarations;
    if (decls && decls->type == NODE_DECLARATIONS) {
        for (size_t i = 0; i < decls->data.declarations.count; i++) {
            ASTNode *decl = decls->data.declarations.items[i];
            if (!semantic_check_declaration(analyzer, decl)) {
                result.is_valid = 0;
                strcpy(result.error_message, analyzer->last_error);
                if (decl) {
                    result.error_line = decl->line;
                    result.error_column = decl->column;
                }
                return result;
            }
        }
    }
    
    /* Process commands */
    ASTNode *cmds = program->data.program.commands;
    if (cmds && cmds->type == NODE_COMMANDS) {
        for (size_t i = 0; i < cmds->data.commands.count; i++) {
            ASTNode *cmd = cmds->data.commands.items[i];
            if (!semantic_check_statement(analyzer, cmd)) {
                result.is_valid = 0;
                strcpy(result.error_message, analyzer->last_error);
                if (cmd) {
                    result.error_line = cmd->line;
                    result.error_column = cmd->column;
                }
                return result;
            }
        }
    }
    
    return result;
}

/* Get type of an expression */
int semantic_get_type(SemanticAnalyzer *analyzer, ASTNode *node) {
    if (!node) return TYPE_UNKNOWN;
    
    switch (node->type) {
        case NODE_NUM_INT:
            return TYPE_INT;
        case NODE_NUM_FLOAT:
            return TYPE_FLOAT;
        case NODE_ID: {
            Symbol *sym = symbol_table_lookup(analyzer->symbols, node->data.id.name);
            return sym ? sym->type : TYPE_UNKNOWN;
        }
        default:
            return TYPE_UNKNOWN;
    }
}
