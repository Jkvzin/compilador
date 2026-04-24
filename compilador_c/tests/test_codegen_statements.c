#include "../src/codegen.h"
#include "../src/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void test_assignment_emit() {
    printf("\n=== Test: Assignment emission ===\n");
    const char *asm_path = ".obj/test_codegen_stmt_assign.asm";
    codegen_init(asm_path);

    ASTNode *decls = ast_make_declarations();
    ast_add_child(decls, ast_make_declaration(TYPE_INT, "x"));
    ASTNode *cmds = ast_make_commands();
    ast_add_child(cmds, ast_make_assignment("x", ast_make_num_int(123)));

    ASTNode *prog = ast_make_program("t", decls, cmds);
    codegen_emit_program(prog);
    codegen_emit_commands(prog->data.program.commands);
    codegen_finish();

    char *s = read_file_to_buffer(asm_path);
    assert_true(s && strstr(s, "mov [rel var_x], eax") != NULL, "store to var_x uses mov [rel var_x], eax");

    free(s);
    ast_free(prog);
}

void test_if_else_labels() {
    printf("\n=== Test: If/Else labels and jumps ===\n");
    const char *asm_path = ".obj/test_codegen_stmt_if_else.asm";
    codegen_init(asm_path);

    ASTNode *decls = ast_make_declarations();
    ast_add_child(decls, ast_make_declaration(TYPE_INT, "a"));
    ast_add_child(decls, ast_make_declaration(TYPE_INT, "b"));

    /* if (a < 10) then b = 1 else b = 0 */
    ASTNode *cond = ast_make_binop(ast_make_id("a"), '<', ast_make_num_int(10));
    ASTNode *then_cmds = ast_make_commands();
    ast_add_child(then_cmds, ast_make_assignment("b", ast_make_num_int(1)));
    ASTNode *else_cmds = ast_make_commands();
    ast_add_child(else_cmds, ast_make_assignment("b", ast_make_num_int(0)));
    ASTNode *ifnode = ast_make_if(cond, then_cmds, else_cmds);

    ASTNode *cmds = ast_make_commands();
    ast_add_child(cmds, ifnode);

    ASTNode *prog = ast_make_program("t", decls, cmds);
    codegen_emit_program(prog);
    codegen_emit_commands(prog->data.program.commands);
    codegen_finish();

    char *s = read_file_to_buffer(asm_path);
    /* Expect at least one unconditional jump to end (jmp .L) */
    assert_true(s && strstr(s, "jmp .L") != NULL, "contains 'jmp .L' jump to endif");

    free(s);
    ast_free(prog);
}

void test_while_labels() {
    printf("\n=== Test: While labels and loop ===\n");
    const char *asm_path = ".obj/test_codegen_stmt_while.asm";
    codegen_init(asm_path);

    ASTNode *decls = ast_make_declarations();
    ast_add_child(decls, ast_make_declaration(TYPE_INT, "a"));

    /* while (a < 5) a = a + 1; */
    ASTNode *cond = ast_make_binop(ast_make_id("a"), '<', ast_make_num_int(5));
    ASTNode *body = ast_make_commands();
    ASTNode *inc = ast_make_assignment("a", ast_make_binop(ast_make_id("a"), '+', ast_make_num_int(1)));
    ast_add_child(body, inc);
    ASTNode *wh = ast_make_while(cond, body);

    ASTNode *cmds = ast_make_commands();
    ast_add_child(cmds, wh);

    ASTNode *prog = ast_make_program("t", decls, cmds);
    codegen_emit_program(prog);
    codegen_emit_commands(prog->data.program.commands);
    codegen_finish();

    char *s = read_file_to_buffer(asm_path);
    assert_true(s && strstr(s, "jmp .L") != NULL, "while loop contains jump to start");

    free(s);
    ast_free(prog);
}

void test_runtime_calls() {
    printf("\n=== Test: Read/Write runtime calls ===\n");
    const char *asm_path = ".obj/test_codegen_stmt_runtime_io.asm";
    codegen_init(asm_path);

    ASTNode *decls = ast_make_declarations();
    ast_add_child(decls, ast_make_declaration(TYPE_INT, "x"));

    ASTNode *cmds = ast_make_commands();
    ast_add_child(cmds, ast_make_read("x"));
    ast_add_child(cmds, ast_make_write(ast_make_id("x"), 0));

    ASTNode *prog = ast_make_program("t", decls, cmds);
    codegen_emit_program(prog);
    codegen_emit_commands(prog->data.program.commands);
    codegen_finish();

    char *s = read_file_to_buffer(asm_path);
    assert_true(s && strstr(s, "call __read_int") != NULL, "calls __read_int for integer read");
    assert_true(s && strstr(s, "call __write_int") != NULL, "calls __write_int for integer write");

    free(s);
    ast_free(prog);
}

int main(void) {
    printf("====== Codegen Statement Tests ======\n");
    test_assignment_emit();
    test_if_else_labels();
    test_while_labels();
    test_runtime_calls();

    printf("\n====== Test Summary ======\n");
    printf("Passed: %d / %d\n", test_passed, test_count);
    return (test_passed == test_count) ? 0 : 1;
}
