#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "ast.h"

/* Parser error handler callback type */
typedef void (*ParserErrorHandler)(const char *message);

/* Parser structure */
typedef struct {
    TokenList *tokens;              /* List of tokens from lexer */
    size_t current;                 /* Current token index */
    ParserErrorHandler error_handler; /* Optional error callback */
} Parser;

/* Create and initialize a parser with a token list */
Parser *parser_create(TokenList *tokens);

/* Free parser resources */
void parser_free(Parser *parser);

/* Main parsing function - parse entire program */
ASTNode *parser_parse_program(Parser *parser);

/* Parse declarations section */
ASTNode *parser_parse_declarations(Parser *parser);

/* Parse a single declaration */
ASTNode *parser_parse_declaration(Parser *parser);

/* Parse commands section */
ASTNode *parser_parse_commands(Parser *parser);

/* Parse a single command - skeleton for now */
ASTNode *parser_parse_command(Parser *parser);

/* Statement parsing functions (Task 8 - not yet implemented) */
/* ASTNode *parser_parse_assignment(Parser *parser);
ASTNode *parser_parse_read(Parser *parser);
ASTNode *parser_parse_write(Parser *parser);
ASTNode *parser_parse_writeln(Parser *parser);
ASTNode *parser_parse_if_statement(Parser *parser);
ASTNode *parser_parse_while_statement(Parser *parser);
ASTNode *parser_parse_for_statement(Parser *parser); */

/* Expression parsing functions */
ASTNode *parser_parse_expression(Parser *parser);
ASTNode *parser_parse_comparison(Parser *parser);
ASTNode *parser_parse_term(Parser *parser);
ASTNode *parser_parse_factor(Parser *parser);
ASTNode *parser_parse_unary(Parser *parser);
ASTNode *parser_parse_primary(Parser *parser);

/* Helper functions */

/* Get current token without advancing */
Token *parser_current_token(Parser *parser);

/* Check if current token matches type and advance if true */
int parser_match(Parser *parser, TokenType type);

/* Check if current token is of given type without advancing */
int parser_check(Parser *parser, TokenType type);

/* Move to next token */
void parser_advance(Parser *parser);

/* Report a parser error */
void parser_error(Parser *parser, const char *message);

#endif /* PARSER_H */
