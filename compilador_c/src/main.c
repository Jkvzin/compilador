/* CLI entrypoint: simplesc compile <input.simp> <out.asm>
 * Wires lexer -> parser -> semantic -> codegen
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"
#include "ast.h"
#include "token.h"

static void print_usage(const char *prog) {
    printf("Compilador SIMPLES -> NASM x86-32\n");
    printf("Uso:\n");
    printf("  %s compile <input.simp> <out.asm>\n", prog);
    printf("  %s help\n", prog);
}

static int read_file_to_string(const char *path, char **out) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        perror(path);
        return 0;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        perror("fseek");
        fclose(f);
        return 0;
    }
    long sz = ftell(f);
    if (sz < 0) {
        perror("ftell");
        fclose(f);
        return 0;
    }
    rewind(f);
    char *buf = malloc((size_t)sz + 1);
    if (!buf) {
        fprintf(stderr, "Out of memory\n");
        fclose(f);
        return 0;
    }
    size_t n = fread(buf, 1, (size_t)sz, f);
    if (n != (size_t)sz) {
        fprintf(stderr, "Failed to read file '%s'\n", path);
        free(buf);
        fclose(f);
        return 0;
    }
    buf[sz] = '\0';
    fclose(f);
    *out = buf;
    return 1;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char *cmd = argv[1];

    if (strcmp(cmd, "help") == 0 || strcmp(cmd, "--help") == 0 || strcmp(cmd, "-h") == 0) {
        print_usage(argv[0]);
        return 0;
    }

    if (strcmp(cmd, "compile") == 0 || strcmp(cmd, "emit-asm") == 0) {
        if (argc != 4) {
            print_usage(argv[0]);
            return 1;
        }

        const char *inpath = argv[2];
        const char *outpath = argv[3];
        printf("Compilando '%s' -> '%s'\n", inpath, outpath);

        /* 1. Ler arquivo fonte */
        char *source = NULL;
        if (!read_file_to_string(inpath, &source)) {
            return 1;
        }

        /* 2. Análise léxica */
        TokenList *tokens = lexer_tokenize(source);
        if (!tokens) {
            fprintf(stderr, "Erro: analise lexica falhou\n");
            free(source);
            return 1;
        }

        /* 3. Análise sintática */
        Parser *parser = parser_create(tokens);
        if (!parser) {
            fprintf(stderr, "Erro: falha ao criar parser\n");
            token_list_free(tokens);
            free(source);
            return 1;
        }

        ASTNode *program = parser_parse_program(parser);
        if (!program) {
            fprintf(stderr, "Erro: analise sintatica falhou\n");
            parser_free(parser);
            token_list_free(tokens);
            free(source);
            return 1;
        }

        parser_free(parser);
        token_list_free(tokens);
        free(source);

        /* 4. Análise semântica */
        SemanticAnalyzer *an = semantic_create();
        if (!an) {
            fprintf(stderr, "Erro: falha ao inicializar analisador semantico\n");
            ast_free(program);
            return 1;
        }

        SemanticResult sr = semantic_analyze_program(an, program);
        if (!sr.is_valid) {
            fprintf(stderr, "Erro semantico: %s (linha %d, coluna %d)\n",
                    sr.error_message, sr.error_line, sr.error_column);
            semantic_free(an);
            ast_free(program);
            return 1;
        }

        /* 5. Geração de código */
        codegen_init(outpath);
        codegen_emit_program(program);
        codegen_finish();

        printf("Sucesso: assembly escrito em %s\n", outpath);

        semantic_free(an);
        ast_free(program);
        return 0;
    }

    print_usage(argv[0]);
    return 1;
}