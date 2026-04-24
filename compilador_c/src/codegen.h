#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

/* Simple global codegen interface (Task 16)
 * - codegen_init opens an assembly output file
 * - codegen_emit_program emits .data and .text prologue for the AST
 * - codegen_emit_expression / statement / commands emit into the open file
 * - codegen_finish writes epilogue, closes file and frees resources
 *
 * Expression results:
 *  - integers end in EAX
 *  - floats end in ST0 (x87 FPU)
 */

/* Initialize the code generator and open the output assembly file. If the
 * directory does not exist, a best-effort attempt is made to create it.
 */
void codegen_init(const char *out_asm_path);

/* Emit program-level constructs (data section and text prologue). Call
 * codegen_emit_commands() to emit the program commands after this.
 */
void codegen_emit_program(ASTNode *program);

/* Emit assembly for an expression. After this call:
 *  - integer expressions leave result in EAX
 *  - floating expressions leave result in ST0
 */
void codegen_emit_expression(ASTNode *expr);

/* Emit statement-level constructs (assign/read/write/if/while/for/commands) */
void codegen_emit_statement(ASTNode *stmt);
void codegen_emit_commands(ASTNode *commands);

/* Finish emission (epilogue), close file and free resources */
void codegen_finish(void);

#endif /* CODEGEN_H */
