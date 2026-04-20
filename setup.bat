@echo off
REM Create directories
if not exist src mkdir src
if not exist tests mkdir tests
if not exist unity mkdir unity

REM Create token.h
(
echo #ifndef TOKEN_H
echo #define TOKEN_H
echo.
echo #include ^<stddef.h^>
echo.
echo typedef enum {
echo     /* Keywords (27) */
echo     TOK_PROGRAMA,
echo     TOK_INICIO,
echo     TOK_FIM,
echo     TOK_INTEIRO,
echo     TOK_FLUTUANTE,
echo     TOK_VAZIO,
echo     TOK_SE,
echo     TOK_ENTAO,
echo     TOK_SENAO,
echo     TOK_FIMSE,
echo     TOK_ENQUANTO,
echo     TOK_FIMENQUANTO,
echo     TOK_PARA,
echo     TOK_DE,
echo     TOK_ATE,
echo     TOK_PASSO,
echo     TOK_FACA,
echo     TOK_FIMPARA,
echo     TOK_LEIA,
echo     TOK_ESCREVA,
echo     TOK_ESCREVAL,
echo     TOK_E,
echo     TOK_OU,
echo     TOK_NAO,
echo     TOK_DIV,
echo     TOK_PROCEDIMENTO,
echo     TOK_RETORNA,
echo.
echo     /* Operators (14) */
echo     TOK_ATRIB,          /* ^<- */
echo     TOK_PLUS,           /* + */
echo     TOK_MINUS,          /* - */
echo     TOK_MULT,           /* * */
echo     TOK_GT,             /* ^> */
echo     TOK_LT,             /* ^< */
echo     TOK_EQ,             /* = */
echo     TOK_NEQ,            /* ^<^> */
echo     TOK_GTE,            /* ^>= */
echo     TOK_LTE,            /* ^<= */
echo     TOK_LPAREN,         /* ( */
echo     TOK_RPAREN,         /* ) */
echo     TOK_COMMA,          /* , */
echo     TOK_SEMICOLON,      /* ; */
echo.
echo     /* Literals and Identifiers (3) */
echo     TOK_ID,
echo     TOK_NUM_INT,
echo     TOK_NUM_FLOAT,
echo.
echo     /* Special (2) */
echo     TOK_EOF,
echo     TOK_ERROR
echo } TokenType;
echo.
echo typedef union {
echo     int int_value;
echo     float float_value;
echo } TokenValue;
echo.
echo typedef struct {
echo     TokenType type;
echo     char *lexeme;
echo     size_t lexeme_len;
echo     int line;
echo     int column;
echo     TokenValue value;
echo } Token;
echo.
echo typedef struct {
echo     Token *tokens;
echo     size_t count;
echo     size_t capacity;
echo } TokenList;
echo.
echo const char *token_type_name(TokenType type^);
echo.
echo void token_list_free(TokenList *list^);
echo.
echo #endif /* TOKEN_H */
) > src\token.h

echo token.h created

REM Create test_lexer.c
(
echo #include "unity.h"
echo #include "../src/lexer.h"
echo.
echo void setUp(void^) {
echo     /* Empty setup for now */
echo }
echo.
echo void tearDown(void^) {
echo     /* Empty teardown for now */
echo }
echo.
echo void test_lexer_single_integer(void^) {
echo     TokenList *tokens = lexer_tokenize("42"^);
echo.
echo     TEST_ASSERT_NOT_NULL(tokens^);
echo     TEST_ASSERT_EQUAL_INT(2, tokens-^>count^);
echo.
echo     /* First token should be NUM_INT */
echo     TEST_ASSERT_EQUAL_INT(TOK_NUM_INT, tokens-^>tokens[0].type^);
echo     TEST_ASSERT_EQUAL_INT(42, tokens-^>tokens[0].value.int_value^);
echo.
echo     /* Second token should be EOF */
echo     TEST_ASSERT_EQUAL_INT(TOK_EOF, tokens-^>tokens[1].type^);
echo.
echo     token_list_free(tokens^);
echo }
echo.
echo int main(void^) {
echo     return unity_main(^);
echo }
) > tests\test_lexer.c

echo test_lexer.c created
