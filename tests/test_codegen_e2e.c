#include "../src/codegen.h"
#include "../src/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(void) {
    printf("=== Codegen E2E Test ===\n");
    const char *asm_path = ".obj/test_prog.asm";

    /* Build a minimal program: int x; x <- 42; escreva x; */
    ASTNode *decls = ast_make_declarations();
    ast_add_child(decls, ast_make_declaration(TYPE_INT, "x"));

    ASTNode *cmds = ast_make_commands();
    ast_add_child(cmds, ast_make_assignment("x", ast_make_num_int(42)));
    ast_add_child(cmds, ast_make_write(ast_make_id("x"), 1));

    ASTNode *prog = ast_make_program("t", decls, cmds);

    /* Emit assembly to file */
    codegen_init(asm_path);
    codegen_emit_program(prog);
    codegen_emit_commands(prog->data.program.commands);
    codegen_finish();

    /* Read and assert content */
    char *s = read_file_to_buffer(asm_path);
    int ok1 = s && strstr(s, "var_x: dd 0") != NULL;
    int ok2 = s && (strstr(s, "call __write_int") != NULL || strstr(s, "mov eax, 42") != NULL);

    assert_true(ok1, ".data contains var_x: dd 0");
    assert_true(ok2, "contains write call or literal mov eax, 42");

    free(s);
    ast_free(prog);

    return (ok1 && ok2) ? 0 : 1;
}
