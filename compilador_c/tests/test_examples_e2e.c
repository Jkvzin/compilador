#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/semantic.h"
#include "../src/codegen.h"
#include "../src/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(d) _mkdir(d)
#else
#include <sys/stat.h>
#define MKDIR(d) mkdir(d, 0755)
#endif

void assert_true(int cond, const char *msg) {
    if (cond) {
        printf("  ✓ %s\n", msg);
    } else {
        printf("  ✗ %s\n", msg);
    }
}

static char *read_file_to_buffer(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    char *buf = malloc(size + 1);
    if (!buf) { fclose(f); return NULL; }
    fread(buf, 1, size, f);
    buf[size] = '\0';
    fclose(f);
    return buf;
}

static int write_file_from_buffer(const char *path, const char *contents) {
    FILE *f = fopen(path, "wb");
    if (!f) return 0;
    fwrite(contents, 1, strlen(contents), f);
    fclose(f);
    return 1;
}

int main(void) {
    printf("=== Examples E2E Test ===\n");

    /* Ensure examples/ exists and write sample programs if missing. This makes the test
       resilient in environments where the repository examples/ directory wasn't created. */
    const char *examples_dir = "examples";
    MKDIR(examples_dir);

    const char *example_paths[3] = {
        "examples/ex1.simp",
        "examples/ex2.simp",
        "examples/ex3.simp"
    };

    const char *example_contents[3] = {
        "programa Teste\ninteiro x;\ninicio\nx <- 42;\nescreva x;\nfim.\n",
        "programa Soma\ninteiro a;\ninteiro b;\ninteiro s;\ninicio\na <- 2;\nb <- 3;\ns <- a + b;\nescreva s;\nfim.\n",
        "programa FloatTest\nflutuante y;\ninicio\ny <- 1.5;\ny <- y * 2.0;\nescreva y;\nfim.\n"
    };

    for (int i = 0; i < 3; ++i) {
        /* If example file missing, write it so test is self-contained */
        char *buf = read_file_to_buffer(example_paths[i]);
        if (!buf) {
            if (!write_file_from_buffer(example_paths[i], example_contents[i])) {
                printf("⚠️  Could not write example %s - continuing\n", example_paths[i]);
            } else {
                printf("  Wrote missing example: %s\n", example_paths[i]);
            }
        } else {
            free(buf);
        }
    }

    int all_ok = 1;

    for (int i = 0; i < 3; ++i) {
        printf("\n--- Example %d: %s ---\n", i+1, example_paths[i]);

        char *source = read_file_to_buffer(example_paths[i]);
        if (!source) {
            printf("  ✗ Failed to read source file %s\n", example_paths[i]);
            all_ok = 0;
            continue;
        }

        TokenList *tokens = lexer_tokenize(source);
        if (!tokens) {
            printf("  ✗ lexer_tokenize returned NULL\n");
            free(source);
            all_ok = 0;
            continue;
        }

        Parser *parser = parser_create(tokens);
        if (!parser) {
            printf("  ✗ parser_create failed\n");
            token_list_free(tokens);
            free(source);
            all_ok = 0;
            continue;
        }

        ASTNode *program = parser_parse_program(parser);
        parser_free(parser);

        if (!program) {
            printf("  ✗ parser_parse_program failed\n");
            token_list_free(tokens);
            free(source);
            all_ok = 0;
            continue;
        }

        SemanticAnalyzer *sa = semantic_create();
        SemanticResult res = semantic_analyze_program(sa, program);
        if (!res.is_valid) {
            printf("  ✗ Semantic error: %s (line %d col %d)\n", res.error_message, res.error_line, res.error_column);
            semantic_free(sa);
            ast_free(program);
            token_list_free(tokens);
            free(source);
            all_ok = 0;
            continue;
        }

        /* Emit assembly to .obj/example_N.asm */
        char asm_path[260];
        snprintf(asm_path, sizeof(asm_path), ".obj/example_%d.asm", i+1);

        codegen_init(asm_path);
        codegen_emit_program(program);
        /* Emit commands explicitly to mirror other tests */
        codegen_emit_commands(program->data.program.commands);
        codegen_finish();

        /* Read and assert content */
        char *asm_s = read_file_to_buffer(asm_path);
        int ok_section = asm_s && (strstr(asm_s, "section .text") != NULL);
        int ok_var = asm_s && (strstr(asm_s, "var_") != NULL || strstr(asm_s, "var_") != NULL);
        int ok_write = 0;
        if (asm_s) {
            if (strstr(asm_s, "call __write_int") || strstr(asm_s, "call __write_float") || strstr(asm_s, "mov eax") || strstr(asm_s, "fld") ) ok_write = 1;
        }

        assert_true(ok_section, "contains section .text");
        assert_true(ok_var, "contains var_ data for declared variables");
        assert_true(ok_write, "contains a write call or literal mov/fld for constants");

        if (!(ok_section && ok_var && ok_write)) {
            all_ok = 0;
        }

        free(asm_s);
        ast_free(program);
        token_list_free(tokens);
        semantic_free(sa);
        free(source);
    }

    printf("\nSummary: %s\n", all_ok ? "All examples passed" : "Some examples failed");
    return all_ok ? 0 : 1;
}
