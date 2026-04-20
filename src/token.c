#include "token.h"
#include <string.h>
#include <stdlib.h>

static const char *token_names[] = {
    /* Keywords */
    "PROGRAMA", "INICIO", "FIM", "INTEIRO", "FLUTUANTE", "VAZIO",
    "SE", "ENTAO", "SENAO", "FIMSE", "ENQUANTO", "FIMENQUANTO",
    "PARA", "DE", "ATE", "PASSO", "FACA", "FIMPARA",
    "LEIA", "ESCREVA", "ESCREVAL", "ESCREVERLN", "E", "OU", "NAO", "DIV",
    "PROCEDIMENTO", "RETORNA",
    
    /* Operators */
    "ATRIB", "PLUS", "MINUS", "MULT", "GT", "LT", "EQ", "NEQ",
    "GTE", "LTE", "LPAREN", "RPAREN", "COMMA", "SEMICOLON", "PERIOD",
    
    /* Literals and Identifiers */
    "ID", "NUM_INT", "NUM_FLOAT",
    
    /* Special */
    "EOF", "ERROR"
};

const char *token_type_name(TokenType type) {
    if (type >= 0 && type < TOK_COUNT) {
        return token_names[type];
    }
    return "UNKNOWN";
}

void token_list_free(TokenList *list) {
    if (!list) return;
    
    if (list->tokens) {
        for (size_t i = 0; i < list->count; i++) {
            if (list->tokens[i].lexeme) {
                free(list->tokens[i].lexeme);
            }
        }
        free(list->tokens);
    }
    
    free(list);
}
