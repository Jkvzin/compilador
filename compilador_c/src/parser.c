#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Create a new parser */
Parser *parser_create(TokenList *tokens) {
    Parser *parser = malloc(sizeof(Parser));
    if (!parser) return NULL;
    
    parser->tokens = tokens;
    parser->current = 0;
    parser->error_handler = NULL;
    
    return parser;
}

/* Free parser resources */
void parser_free(Parser *parser) {
    if (parser) {
        free(parser);
    }
}

/* Get current token without advancing */
Token *parser_current_token(Parser *parser) {
    if (!parser || !parser->tokens || parser->current >= parser->tokens->count) {
        return NULL;
    }
    return &parser->tokens->tokens[parser->current];
}

/* Check if current token is of given type */
int parser_check(Parser *parser, TokenType type) {
    Token *token = parser_current_token(parser);
    if (!token) return 0;
    return token->type == type;
}

/* Check if current token matches type and advance if true */
int parser_match(Parser *parser, TokenType type) {
    if (parser_check(parser, type)) {
        parser_advance(parser);
        return 1;
    }
    return 0;
}

/* Move to next token */
void parser_advance(Parser *parser) {
    if (parser && parser->current < parser->tokens->count) {
        parser->current++;
    }
}

/* Report a parser error */
void parser_error(Parser *parser, const char *message) {
    Token *token = parser_current_token(parser);
    if (token) {
        fprintf(stderr, "Parse error at line %d, column %d: %s\n", 
                token->line, token->column, message);
    } else {
        fprintf(stderr, "Parse error: %s\n", message);
    }
    
    if (parser->error_handler) {
        parser->error_handler(message);
    }
}

/* Parse entire program */
ASTNode *parser_parse_program(Parser *parser) {
    if (!parser) return NULL;
    
    /* Expect 'programa' keyword */
    if (!parser_match(parser, TOK_PROGRAMA)) {
        parser_error(parser, "Expected 'programa' keyword");
        return NULL;
    }
    
    /* Get program name (identifier) */
    if (!parser_check(parser, TOK_ID)) {
        parser_error(parser, "Expected program name (identifier)");
        return NULL;
    }
    
    Token *name_token = parser_current_token(parser);
    char *program_name = malloc(name_token->lexeme_len + 1);
    if (!program_name) return NULL;
    strncpy(program_name, name_token->lexeme, name_token->lexeme_len);
    program_name[name_token->lexeme_len] = '\0';
    
    parser_advance(parser);
    
    /* Parse declarations */
    ASTNode *declarations = parser_parse_declarations(parser);
    if (!declarations) {
        free(program_name);
        return NULL;
    }
    
    /* Expect 'inicio' keyword */
    if (!parser_match(parser, TOK_INICIO)) {
        parser_error(parser, "Expected 'inicio' keyword");
        free(program_name);
        ast_free(declarations);
        return NULL;
    }
    
    /* Parse commands */
    ASTNode *commands = parser_parse_commands(parser);
    if (!commands) {
        free(program_name);
        ast_free(declarations);
        return NULL;
    }
    
    /* Expect 'fim' keyword */
    if (!parser_match(parser, TOK_FIM)) {
        parser_error(parser, "Expected 'fim' keyword");
        free(program_name);
        ast_free(declarations);
        ast_free(commands);
        return NULL;
    }
    
    /* Expect '.' (period) */
    if (!parser_match(parser, TOK_PERIOD)) {
        parser_error(parser, "Expected '.' (period) after 'fim'");
        free(program_name);
        ast_free(declarations);
        ast_free(commands);
        return NULL;
    }
    
    /* Expect EOF */
    if (!parser_check(parser, TOK_EOF)) {
        parser_error(parser, "Expected end of file");
        free(program_name);
        ast_free(declarations);
        ast_free(commands);
        return NULL;
    }
    
    /* Create program node */
    ASTNode *program = ast_make_program(program_name, declarations, commands);
    free(program_name);
    
    if (program) {
        program->line = name_token->line;
        program->column = name_token->column;
    }
    
    return program;
}

/* Parse declarations section */
ASTNode *parser_parse_declarations(Parser *parser) {
    if (!parser) return NULL;
    
    ASTNode *declarations = ast_make_declarations();
    if (!declarations) return NULL;
    
    /* Parse while we see type keywords (inteiro or flutuante) */
    while (parser_check(parser, TOK_INTEIRO) || parser_check(parser, TOK_FLUTUANTE)) {
        ASTNode *decl = parser_parse_declaration(parser);
        if (!decl) {
            ast_free(declarations);
            return NULL;
        }
        
        /* If multi-variable declaration returned a DECLARATIONS node,
         * flatten its children into our list */
        if (decl->type == NODE_DECLARATIONS) {
            for (size_t i = 0; i < decl->data.declarations.count; i++) {
                ast_add_child(declarations, decl->data.declarations.items[i]);
                decl->data.declarations.items[i] = NULL; /* prevent double free */
            }
            ast_free(decl);
        } else {
            ast_add_child(declarations, decl);
        }
    }
    
    return declarations;
}

/* Parse a single declaration — supports: inteiro a, b, c;
 * Returns the first declaration node. Additional comma-separated
 * variables are added directly to the parent declarations list via
 * a helper approach: we parse them here and return a temporary
 * DECLARATIONS node containing all of them.
 */
ASTNode *parser_parse_declaration(Parser *parser) {
    if (!parser) return NULL;
    
    /* Get type */
    DataType type;
    Token *type_token = NULL;
    
    if (parser_match(parser, TOK_INTEIRO)) {
        type = TYPE_INT;
        type_token = &parser->tokens->tokens[parser->current - 1];
    } else if (parser_match(parser, TOK_FLUTUANTE)) {
        type = TYPE_FLOAT;
        type_token = &parser->tokens->tokens[parser->current - 1];
    } else {
        parser_error(parser, "Expected type (inteiro or flutuante)");
        return NULL;
    }
    
    /* Get first identifier */
    if (!parser_check(parser, TOK_ID)) {
        parser_error(parser, "Expected variable name (identifier)");
        return NULL;
    }
    
    Token *id_token = parser_current_token(parser);
    char *var_name = malloc(id_token->lexeme_len + 1);
    if (!var_name) return NULL;
    strncpy(var_name, id_token->lexeme, id_token->lexeme_len);
    var_name[id_token->lexeme_len] = '\0';
    parser_advance(parser);
    
    /* Create first declaration node */
    ASTNode *first_decl = ast_make_declaration(type, var_name);
    free(var_name);
    if (first_decl) {
        first_decl->line = type_token->line;
        first_decl->column = type_token->column;
    }
    
    /* Check for additional comma-separated variables: inteiro a, b, c; */
    /* We wrap multiple declarations in a temporary DECLARATIONS node */
    if (parser_check(parser, TOK_COMMA)) {
        ASTNode *multi = ast_make_declarations();
        if (!multi) {
            ast_free(first_decl);
            return NULL;
        }
        ast_add_child(multi, first_decl);
        
        while (parser_match(parser, TOK_COMMA)) {
            if (!parser_check(parser, TOK_ID)) {
                parser_error(parser, "Expected variable name after ','");
                ast_free(multi);
                return NULL;
            }
            Token *next_id = parser_current_token(parser);
            char *next_name = malloc(next_id->lexeme_len + 1);
            if (!next_name) { ast_free(multi); return NULL; }
            strncpy(next_name, next_id->lexeme, next_id->lexeme_len);
            next_name[next_id->lexeme_len] = '\0';
            parser_advance(parser);
            
            ASTNode *d = ast_make_declaration(type, next_name);
            free(next_name);
            if (d) {
                d->line = next_id->line;
                d->column = next_id->column;
            }
            ast_add_child(multi, d);
        }
        
        /* Expect semicolon */
        if (!parser_match(parser, TOK_SEMICOLON)) {
            parser_error(parser, "Expected ';' after variable declaration");
            ast_free(multi);
            return NULL;
        }
        
        return multi; /* DECLARATIONS node containing all vars */
    }
    
    /* Single variable — expect semicolon */
    if (!parser_match(parser, TOK_SEMICOLON)) {
        parser_error(parser, "Expected ';' after variable declaration");
        ast_free(first_decl);
        return NULL;
    }
    
    return first_decl;
}

/* Parse commands section */
ASTNode *parser_parse_commands(Parser *parser) {
    if (!parser) return NULL;
    
    ASTNode *commands = ast_make_commands();
    if (!commands) return NULL;
    
    /* Parse commands until we see a terminator (fim, fimse, fimenquanto, fimpara) or EOF */
    while (!parser_check(parser, TOK_FIM) && 
           !parser_check(parser, TOK_FIMSE) &&
           !parser_check(parser, TOK_SENAO) &&
           !parser_check(parser, TOK_FIMENQUANTO) &&
           !parser_check(parser, TOK_FIMPARA) &&
           !parser_check(parser, TOK_EOF)) {
        ASTNode *cmd = parser_parse_command(parser);
        if (!cmd) {
            /* Skeleton implementation - skip unknown commands for now */
            parser_advance(parser);
            continue;
        }
        
        ast_add_child(commands, cmd);
    }
    
    return commands;
}

/* Parse a read statement: leia var */
ASTNode *parser_parse_read(Parser *parser) {
    if (!parser) return NULL;
    
    Token *read_token = parser_current_token(parser);
    
    /* Match 'leia' */
    if (!parser_match(parser, TOK_LEIA)) {
        return NULL;
    }
    
    /* Get variable name */
    Token *id_token = parser_current_token(parser);
    if (!id_token || id_token->type != TOK_ID) {
        parser_error(parser, "Expected variable name after 'leia'");
        return NULL;
    }
    
    char *var_name = malloc(id_token->lexeme_len + 1);
    if (!var_name) return NULL;
    strncpy(var_name, id_token->lexeme, id_token->lexeme_len);
    var_name[id_token->lexeme_len] = '\0';
    
    parser_advance(parser);
    
    /* Expect semicolon */
    if (!parser_match(parser, TOK_SEMICOLON)) {
        parser_error(parser, "Expected ';' after read statement");
        free(var_name);
        return NULL;
    }
    
    /* Create read node */
    ASTNode *node = ast_make_read(var_name);
    if (node) {
        node->line = read_token->line;
        node->column = read_token->column;
    }
    free(var_name);
    return node;
}

/* Parse a write statement: escreva expr */
ASTNode *parser_parse_write(Parser *parser) {
    if (!parser) return NULL;
    
    Token *write_token = parser_current_token(parser);
    
    if (!parser_match(parser, TOK_ESCREVA)) {
        return NULL;
    }
    
    /* Parse expression */
    ASTNode *expr = parser_parse_expression(parser);
    if (!expr) {
        parser_error(parser, "Expected expression after 'escreva'");
        return NULL;
    }
    
    /* Expect semicolon */
    if (!parser_match(parser, TOK_SEMICOLON)) {
        parser_error(parser, "Expected ';' after write statement");
        ast_free(expr);
        return NULL;
    }
    
    /* Create write node (no newline) */
    ASTNode *node = ast_make_write(expr, 0);
    if (node) {
        node->line = write_token->line;
        node->column = write_token->column;
    }
    return node;
}

/* Parse a writeln statement: escreverln expr */
ASTNode *parser_parse_writeln(Parser *parser) {
    if (!parser) return NULL;
    
    Token *writeln_token = parser_current_token(parser);
    
    /* Check for both TOK_ESCREVAL and TOK_ESCREVERLN for backward compatibility */
    if (parser_match(parser, TOK_ESCREVERLN)) {
        /* Token matched, continue */
    } else if (parser_match(parser, TOK_ESCREVAL)) {
        /* Token matched, continue */
    } else {
        return NULL;
    }
    
    /* Parse expression */
    ASTNode *expr = parser_parse_expression(parser);
    if (!expr) {
        parser_error(parser, "Expected expression after 'escreverln'");
        return NULL;
    }
    
    /* Expect semicolon */
    if (!parser_match(parser, TOK_SEMICOLON)) {
        parser_error(parser, "Expected ';' after writeln statement");
        ast_free(expr);
        return NULL;
    }
    
    /* Create write node (with newline) */
    ASTNode *node = ast_make_write(expr, 1);
    if (node) {
        node->line = writeln_token->line;
        node->column = writeln_token->column;
    }
    return node;
}

/* Parse an assignment statement: var <- expression */
ASTNode *parser_parse_assignment(Parser *parser) {
    if (!parser) return NULL;
    
    Token *id_token = parser_current_token(parser);
    if (!id_token || id_token->type != TOK_ID) {
        parser_error(parser, "Expected identifier");
        return NULL;
    }
    
    char *var_name = malloc(id_token->lexeme_len + 1);
    if (!var_name) return NULL;
    strncpy(var_name, id_token->lexeme, id_token->lexeme_len);
    var_name[id_token->lexeme_len] = '\0';
    
    parser_advance(parser);
    
    /* Expect assignment operator (<-) */
    if (!parser_match(parser, TOK_ATRIB)) {
        parser_error(parser, "Expected '<-' in assignment");
        free(var_name);
        return NULL;
    }
    
    /* Parse expression */
    ASTNode *expr = parser_parse_expression(parser);
    if (!expr) {
        parser_error(parser, "Expected expression after '<-'");
        free(var_name);
        return NULL;
    }
    
    /* Expect semicolon */
    if (!parser_match(parser, TOK_SEMICOLON)) {
        parser_error(parser, "Expected ';' after assignment");
        free(var_name);
        ast_free(expr);
        return NULL;
    }
    
    /* Create assignment node */
    ASTNode *node = ast_make_assignment(var_name, expr);
    if (node) {
        node->line = id_token->line;
        node->column = id_token->column;
    }
    free(var_name);
    return node;
}

/* Parse an if statement: se condition entao ... [senao ...] fimse */
ASTNode *parser_parse_if_statement(Parser *parser) {
    if (!parser) return NULL;
    
    Token *if_token = parser_current_token(parser);
    
    if (!parser_match(parser, TOK_SE)) {
        return NULL;
    }
    
    /* Parse condition */
    ASTNode *condition = parser_parse_expression(parser);
    if (!condition) {
        parser_error(parser, "Expected condition after 'se'");
        return NULL;
    }
    
    /* Expect 'entao' */
    if (!parser_match(parser, TOK_ENTAO)) {
        parser_error(parser, "Expected 'entao' after if condition");
        ast_free(condition);
        return NULL;
    }
    
    /* Parse then branch */
    ASTNode *then_branch = parser_parse_commands(parser);
    if (!then_branch) {
        ast_free(condition);
        return NULL;
    }
    
    /* Check for else */
    ASTNode *else_branch = NULL;
    if (parser_check(parser, TOK_SENAO)) {
        parser_advance(parser);
        else_branch = parser_parse_commands(parser);
        if (!else_branch) {
            ast_free(condition);
            ast_free(then_branch);
            return NULL;
        }
    }
    
    /* Expect 'fimse' */
    if (!parser_match(parser, TOK_FIMSE)) {
        parser_error(parser, "Expected 'fimse'");
        ast_free(condition);
        ast_free(then_branch);
        ast_free(else_branch);
        return NULL;
    }
    
    /* Create if node */
    ASTNode *node = ast_make_if(condition, then_branch, else_branch);
    if (node) {
        node->line = if_token->line;
        node->column = if_token->column;
    }
    return node;
}

/* Parse a while loop: enquanto condition faca ... fimenquanto */
ASTNode *parser_parse_while_statement(Parser *parser) {
    if (!parser) return NULL;
    
    Token *while_token = parser_current_token(parser);
    
    if (!parser_match(parser, TOK_ENQUANTO)) {
        return NULL;
    }
    
    /* Parse condition */
    ASTNode *condition = parser_parse_expression(parser);
    if (!condition) {
        parser_error(parser, "Expected condition after 'enquanto'");
        return NULL;
    }
    
    /* Expect 'faca' */
    if (!parser_match(parser, TOK_FACA)) {
        parser_error(parser, "Expected 'faca' after while condition");
        ast_free(condition);
        return NULL;
    }
    
    /* Parse body */
    ASTNode *body = parser_parse_commands(parser);
    if (!body) {
        ast_free(condition);
        return NULL;
    }
    
    /* Expect 'fimenquanto' */
    if (!parser_match(parser, TOK_FIMENQUANTO)) {
        parser_error(parser, "Expected 'fimenquanto'");
        ast_free(condition);
        ast_free(body);
        return NULL;
    }
    
    /* Create while node */
    ASTNode *node = ast_make_while(condition, body);
    if (node) {
        node->line = while_token->line;
        node->column = while_token->column;
    }
    return node;
}

/* Parse a for loop: para var de inicio ate fim passo step faca ... fimpara */
ASTNode *parser_parse_for_statement(Parser *parser) {
    if (!parser) return NULL;
    
    Token *for_token = parser_current_token(parser);
    
    if (!parser_match(parser, TOK_PARA)) {
        return NULL;
    }
    
    /* Get loop variable */
    Token *id_token = parser_current_token(parser);
    if (!id_token || id_token->type != TOK_ID) {
        parser_error(parser, "Expected loop variable in for");
        return NULL;
    }
    
    char *var_name = malloc(id_token->lexeme_len + 1);
    if (!var_name) return NULL;
    strncpy(var_name, id_token->lexeme, id_token->lexeme_len);
    var_name[id_token->lexeme_len] = '\0';
    
    parser_advance(parser);
    
    /* Expect 'de' */
    if (!parser_match(parser, TOK_DE)) {
        parser_error(parser, "Expected 'de' in for loop");
        free(var_name);
        return NULL;
    }
    
    /* Parse start expression */
    ASTNode *start = parser_parse_expression(parser);
    if (!start) {
        parser_error(parser, "Expected start value in for loop");
        free(var_name);
        return NULL;
    }
    
    /* Expect 'ate' */
    if (!parser_match(parser, TOK_ATE)) {
        parser_error(parser, "Expected 'ate' in for loop");
        free(var_name);
        ast_free(start);
        return NULL;
    }
    
    /* Parse end expression */
    ASTNode *end = parser_parse_expression(parser);
    if (!end) {
        parser_error(parser, "Expected end value in for loop");
        free(var_name);
        ast_free(start);
        return NULL;
    }
    
    /* Expect 'passo' */
    if (!parser_match(parser, TOK_PASSO)) {
        parser_error(parser, "Expected 'passo' in for loop");
        free(var_name);
        ast_free(start);
        ast_free(end);
        return NULL;
    }
    
    /* Parse step expression */
    ASTNode *step = parser_parse_expression(parser);
    if (!step) {
        parser_error(parser, "Expected step value in for loop");
        free(var_name);
        ast_free(start);
        ast_free(end);
        return NULL;
    }
    
    /* Expect 'faca' */
    if (!parser_match(parser, TOK_FACA)) {
        parser_error(parser, "Expected 'faca' in for loop");
        free(var_name);
        ast_free(start);
        ast_free(end);
        ast_free(step);
        return NULL;
    }
    
    /* Parse body */
    ASTNode *body = parser_parse_commands(parser);
    if (!body) {
        free(var_name);
        ast_free(start);
        ast_free(end);
        ast_free(step);
        return NULL;
    }
    
    /* Expect 'fimpara' */
    if (!parser_match(parser, TOK_FIMPARA)) {
        parser_error(parser, "Expected 'fimpara'");
        free(var_name);
        ast_free(start);
        ast_free(end);
        ast_free(step);
        ast_free(body);
        return NULL;
    }
    
    /* Create for node */
    ASTNode *node = ast_make_for(var_name, start, end, step, body);
    if (node) {
        node->line = for_token->line;
        node->column = for_token->column;
    }
    free(var_name);
    return node;
}

/* Parse primary expressions (numbers, identifiers, parenthesized expressions) */
ASTNode *parser_parse_primary(Parser *parser) {
    if (!parser) return NULL;
    
    Token *token = parser_current_token(parser);
    if (!token) {
        parser_error(parser, "Unexpected end of input in primary expression");
        return NULL;
    }
    
    /* Parse integer literal */
    if (token->type == TOK_NUM_INT) {
        long long value = token->value.int_value;
        ASTNode *node = ast_make_num_int(value);
        if (node) {
            node->line = token->line;
            node->column = token->column;
        }
        parser_advance(parser);
        return node;
    }
    
    /* Parse float literal */
    if (token->type == TOK_NUM_FLOAT) {
        double value = token->value.float_value;
        ASTNode *node = ast_make_num_float(value);
        if (node) {
            node->line = token->line;
            node->column = token->column;
        }
        parser_advance(parser);
        return node;
    }
    
    /* Parse identifier */
    if (token->type == TOK_ID) {
        char *name = malloc(token->lexeme_len + 1);
        if (!name) return NULL;
        strncpy(name, token->lexeme, token->lexeme_len);
        name[token->lexeme_len] = '\0';
        
        ASTNode *node = ast_make_id(name);
        if (node) {
            node->line = token->line;
            node->column = token->column;
        }
        free(name);
        parser_advance(parser);
        return node;
    }
    
    /* Parse parenthesized expression */
    if (token->type == TOK_LPAREN) {
        parser_advance(parser);
        ASTNode *expr = parser_parse_expression(parser);
        if (!expr) {
            parser_error(parser, "Expected expression after '('");
            return NULL;
        }
        
        if (!parser_match(parser, TOK_RPAREN)) {
            parser_error(parser, "Expected ')' after expression");
            ast_free(expr);
            return NULL;
        }
        
        return expr;
    }
    
    parser_error(parser, "Expected primary expression (number, identifier, or '(')");
    return NULL;
}

/* Parse unary expressions (-, nao) */
ASTNode *parser_parse_unary(Parser *parser) {
    if (!parser) return NULL;
    
    Token *token = parser_current_token(parser);
    if (!token) return NULL;
    
    /* Handle unary minus */
    if (token->type == TOK_MINUS) {
        parser_advance(parser);
        ASTNode *operand = parser_parse_unary(parser);
        if (!operand) {
            parser_error(parser, "Expected expression after unary minus");
            return NULL;
        }
        
        ASTNode *node = ast_make_unop('-', operand);
        if (node) {
            node->line = token->line;
            node->column = token->column;
        }
        return node;
    }
    
    /* Handle logical not (nao) */
    if (token->type == TOK_NAO) {
        parser_advance(parser);
        ASTNode *operand = parser_parse_unary(parser);
        if (!operand) {
            parser_error(parser, "Expected expression after 'nao'");
            return NULL;
        }
        
        ASTNode *node = ast_make_unop('~', operand);
        if (node) {
            node->line = token->line;
            node->column = token->column;
        }
        return node;
    }
    
    return parser_parse_primary(parser);
}

/* Parse factor expressions (*, /) */
ASTNode *parser_parse_factor(Parser *parser) {
    if (!parser) return NULL;
    
    ASTNode *left = parser_parse_unary(parser);
    if (!left) return NULL;
    
    while (1) {
        Token *token = parser_current_token(parser);
        if (!token) break;
        
        char op = '\0';
        if (token->type == TOK_MULT) {
            op = '*';
        } else if (token->type == TOK_DIV) {
            op = '/';
        } else {
            break;
        }
        
        parser_advance(parser);
        
        ASTNode *right = parser_parse_unary(parser);
        if (!right) {
            parser_error(parser, "Expected expression after operator");
            ast_free(left);
            return NULL;
        }
        
        left = ast_make_binop(left, op, right);
        if (left) {
            left->line = token->line;
            left->column = token->column;
        }
    }
    
    return left;
}

/* Parse term expressions (+, -) */
ASTNode *parser_parse_term(Parser *parser) {
    if (!parser) return NULL;
    
    ASTNode *left = parser_parse_factor(parser);
    if (!left) return NULL;
    
    while (1) {
        Token *token = parser_current_token(parser);
        if (!token) break;
        
        char op = '\0';
        if (token->type == TOK_PLUS) {
            op = '+';
        } else if (token->type == TOK_MINUS) {
            op = '-';
        } else {
            break;
        }
        
        parser_advance(parser);
        
        ASTNode *right = parser_parse_factor(parser);
        if (!right) {
            parser_error(parser, "Expected expression after operator");
            ast_free(left);
            return NULL;
        }
        
        left = ast_make_binop(left, op, right);
        if (left) {
            left->line = token->line;
            left->column = token->column;
        }
    }
    
    return left;
}

/* Parse comparison expressions (=, <>, <, >, <=, >=) */
ASTNode *parser_parse_comparison(Parser *parser) {
    if (!parser) return NULL;
    
    ASTNode *left = parser_parse_term(parser);
    if (!left) return NULL;
    
    while (1) {
        Token *token = parser_current_token(parser);
        if (!token) break;
        
        char op = '\0';
        if (token->type == TOK_EQ) {
            op = '=';
        } else if (token->type == TOK_NEQ) {
            /* For multi-character operators, we store the first char and rely on context */
            op = '!';  /* We'll use '!' to represent <> */
        } else if (token->type == TOK_LT) {
            op = '<';
        } else if (token->type == TOK_GT) {
            op = '>';
        } else if (token->type == TOK_LTE) {
            /* For <=, store as special: we'll use '[' as a marker */
            op = '[';
        } else if (token->type == TOK_GTE) {
            /* For >=, store as special: we'll use ']' as a marker */
            op = ']';
        } else {
            break;
        }
        
        parser_advance(parser);
        
        ASTNode *right = parser_parse_term(parser);
        if (!right) {
            parser_error(parser, "Expected expression after comparison operator");
            ast_free(left);
            return NULL;
        }
        
        left = ast_make_binop(left, op, right);
        if (left) {
            left->line = token->line;
            left->column = token->column;
        }
    }
    
    return left;
}

/* Parse logical expressions: expr_relacional { ("e" | "ou") expr_relacional } */
ASTNode *parser_parse_logical(Parser *parser) {
    if (!parser) return NULL;
    
    ASTNode *left = parser_parse_comparison(parser);
    if (!left) return NULL;
    
    while (1) {
        Token *token = parser_current_token(parser);
        if (!token) break;
        
        char op = '\0';
        if (token->type == TOK_E) {
            op = '&'; /* internal representation for logical AND */
        } else if (token->type == TOK_OU) {
            op = '|'; /* internal representation for logical OR */
        } else {
            break;
        }
        
        parser_advance(parser);
        
        ASTNode *right = parser_parse_comparison(parser);
        if (!right) {
            parser_error(parser, "Expected expression after logical operator");
            ast_free(left);
            return NULL;
        }
        
        left = ast_make_binop(left, op, right);
        if (left) {
            left->line = token->line;
            left->column = token->column;
        }
    }
    
    return left;
}

/* Parse expression (entry point) — delegates to logical */
ASTNode *parser_parse_expression(Parser *parser) {
    if (!parser) return NULL;
    return parser_parse_logical(parser);
}

/* Parse a single command */
ASTNode *parser_parse_command(Parser *parser) {
    if (!parser) return NULL;
    
    Token *current = parser_current_token(parser);
    if (!current) return NULL;
    
    /* Check token type and delegate to specific handlers */
    switch (current->type) {
        case TOK_LEIA:
            return parser_parse_read(parser);
            
        case TOK_ESCREVA:
            return parser_parse_write(parser);
        
        case TOK_ESCREVAL:
        case TOK_ESCREVERLN:
            return parser_parse_writeln(parser);
            
        case TOK_SE:
            return parser_parse_if_statement(parser);
            
        case TOK_ENQUANTO:
            return parser_parse_while_statement(parser);
            
        case TOK_PARA:
            return parser_parse_for_statement(parser);
            
        case TOK_ID:
            return parser_parse_assignment(parser);
            
        default:
            return NULL;
    }
}
