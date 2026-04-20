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

static char *read_stream_to_buffer(FILE *f) {
    if (!f) return NULL;
    fflush(f);
    long saved = ftell(f);
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(size + 1);
    if (!buf) return NULL;
    fread(buf, 1, size, f);
    buf[size] = '\0';
    /* restore position */
    fseek(f, saved, SEEK_SET);
    return buf;
}

void test_num_int_literal() {
    printf("\n=== Test: NUM_INT literal ===\n");
    FILE *out = tmpfile();
    CodeGen *cg = codegen_create(out);

    ASTNode *prog = ast_make_program("t", ast_make_declarations(), ast_make_commands());
    codegen_emit_program(cg, prog);

    ASTNode *n = ast_make_num_int(42);
    codegen_emit_expression(cg, n);
    codegen_finish(cg);

    rewind(out);
    char *s = read_stream_to_buffer(out);
    assert_true(s && strstr(s, "mov eax, 42") != NULL, "mov eax, 42 present");

    free(s);
    codegen_free(cg);
    ast_free(prog);
}

void test_binop_add_ints() {
    printf("\n=== Test: BINOP int add ===\n");
    FILE *out = tmpfile();
    CodeGen *cg = codegen_create(out);
    ASTNode *prog = ast_make_program("t", ast_make_declarations(), ast_make_commands());
    codegen_emit_program(cg, prog);

    ASTNode *l = ast_make_num_int(2);
    ASTNode *r = ast_make_num_int(3);
    ASTNode *add = ast_make_binop(l, '+', r);
    codegen_emit_expression(cg, add);
    codegen_finish(cg);

    rewind(out);
    char *s = read_stream_to_buffer(out);
    assert_true(s && strstr(s, "add eax, ebx") != NULL, "add eax, ebx present");

    free(s);
    codegen_free(cg);
    ast_free(prog);
}

void test_binop_add_floats() {
    printf("\n=== Test: BINOP float add ===\n");
    FILE *out = tmpfile();
    CodeGen *cg = codegen_create(out);
    ASTNode *prog = ast_make_program("t", ast_make_declarations(), ast_make_commands());
    codegen_emit_program(cg, prog);

    ASTNode *l = ast_make_num_float(1.5);
    ASTNode *r = ast_make_num_float(2.25);
    ASTNode *add = ast_make_binop(l, '+', r);
    codegen_emit_expression(cg, add);
    codegen_finish(cg);

    rewind(out);
    char *s = read_stream_to_buffer(out);
    assert_true(s && strstr(s, "faddp st1, st0") != NULL, "faddp st1, st0 present");

    free(s);
    codegen_free(cg);
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
