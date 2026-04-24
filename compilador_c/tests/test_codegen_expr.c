#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/codegen.h"
#include "../src/ast.h"

static int test_count = 0;
static int test_passed = 0;

void assert_true(int cond, const char *msg) {
    test_count++;
    if (cond) {
        printf("  ✓ %s\n", msg);
        test_passed++;
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

void test_num_int_literal() {
    printf("\n=== Test: NUM_INT literal ===\n");
    const char *asm_path = ".obj/test_codegen_expr_num_int.asm";
    codegen_init(asm_path);

    ASTNode *prog = ast_make_program("t", ast_make_declarations(), ast_make_commands());
    codegen_emit_program(prog);

    ASTNode *n = ast_make_num_int(42);
    codegen_emit_expression(n);
    codegen_finish();

    char *s = read_file_to_buffer(asm_path);
    assert_true(s && strstr(s, "mov eax, 42") != NULL, "mov eax, 42 present");

    free(s);
    ast_free(prog);
}

void test_binop_add_ints() {
    printf("\n=== Test: BINOP int add ===\n");
    const char *asm_path = ".obj/test_codegen_expr_add_ints.asm";
    codegen_init(asm_path);
    ASTNode *prog = ast_make_program("t", ast_make_declarations(), ast_make_commands());
    codegen_emit_program(prog);

    ASTNode *l = ast_make_num_int(2);
    ASTNode *r = ast_make_num_int(3);
    ASTNode *add = ast_make_binop(l, '+', r);
    codegen_emit_expression(add);
    codegen_finish();

    char *s = read_file_to_buffer(asm_path);
    assert_true(s && strstr(s, "add eax, ebx") != NULL, "add eax, ebx present");

    free(s);
    ast_free(prog);
}

void test_binop_add_floats() {
    printf("\n=== Test: BINOP float add ===\n");
    const char *asm_path = ".obj/test_codegen_expr_add_floats.asm";
    codegen_init(asm_path);
    ASTNode *prog = ast_make_program("t", ast_make_declarations(), ast_make_commands());
    codegen_emit_program(prog);

    ASTNode *l = ast_make_num_float(1.5);
    ASTNode *r = ast_make_num_float(2.25);
    ASTNode *add = ast_make_binop(l, '+', r);
    codegen_emit_expression(add);
    codegen_finish();

    char *s = read_file_to_buffer(asm_path);
    assert_true(s && strstr(s, "faddp st1, st0") != NULL, "faddp st1, st0 present");

    free(s);
    ast_free(prog);
}

int main(void) {
    printf("====== Codegen Expression Tests ======\n");
    test_num_int_literal();
    test_binop_add_ints();
    test_binop_add_floats();

    printf("\n====== Test Summary ======\n");
    printf("Passed: %d / %d\n", test_passed, test_count);
    return (test_passed == test_count) ? 0 : 1;
}
