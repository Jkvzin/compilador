#include "../src/codegen.h"
#include "../src/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void assert_true(int cond, const char *msg) {
    if (cond) printf("  ✓ %s\n", msg);
    else printf("  ✗ %s\n", msg);
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
    printf("=== Codegen Mixed Types Tests ===\n");
    int all_ok = 1;

    /* Test 1: assign int literal to float variable */
    ASTNode *decls = ast_make_declarations();
    ast_add_child(decls, ast_make_declaration(TYPE_FLOAT, "f"));
    ASTNode *cmds = ast_make_commands();
    ast_add_child(cmds, ast_make_assignment("f", ast_make_num_int(7)));
    ASTNode *prog = ast_make_program("t", decls, cmds);
    const char *asm_path = ".obj/test_codegen_mixed_assign_float.asm";
    codegen_init(asm_path);
    codegen_emit_program(prog);
    codegen_finish();

    char *s = read_file_to_buffer(asm_path);
    int ok1 = s && strstr(s, "var_f: dq 0.0") != NULL;
    int ok2 = s && (strstr(s, "fild") != NULL || strstr(s, "tmp_int_") != NULL) && strstr(s, "fstp qword [rel var_f]") != NULL;
    assert_true(ok1, "data contains var_f as dq 0.0");
    assert_true(ok2, "int->float conversion emitted (fild + fstp)");
    if (!(ok1 && ok2)) all_ok = 0;

    free(s);
    ast_free(prog);

    /* Test 2: mixed expression (int + float) assigned to float var */
    decls = ast_make_declarations();
    ast_add_child(decls, ast_make_declaration(TYPE_FLOAT, "res"));
    cmds = ast_make_commands();
    ASTNode *expr = ast_make_binop(ast_make_num_int(2), '+', ast_make_num_float(3.5));
    ast_add_child(cmds, ast_make_assignment("res", expr));
    prog = ast_make_program("t", decls, cmds);
    asm_path = ".obj/test_codegen_mixed_binop.asm";
    codegen_init(asm_path);
    codegen_emit_program(prog);
    codegen_finish();

    s = read_file_to_buffer(asm_path);
    int ok3 = s && strstr(s, "faddp st1, st0") != NULL;
    int ok4 = s && strstr(s, "fstp qword [rel var_res]") != NULL;
    assert_true(ok3, "float add uses faddp");
    assert_true(ok4, "result stored to var_res as float");
    if (!(ok3 && ok4)) all_ok = 0;

    free(s);
    ast_free(prog);

    printf("\nSummary: %s\n", all_ok ? "OK" : "FAIL");
    return all_ok ? 0 : 1;
}
