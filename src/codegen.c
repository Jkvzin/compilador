#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* Minimal code generator that emits NASM (Intel) x86-32 assembly for
 * expression-level AST nodes. This implementation intentionally keeps
 * the register usage conservative and aims for readable assembly.
 */

typedef struct VarEntry {
    char *name;
    DataType type;
    int defined; /* 1 if declared in program, 0 if implicit */
} VarEntry;

struct CodeGen {
    FILE *out;
    ASTNode *program; /* optional, used to discover declared variables */
    int label_count;

    VarEntry *vars;
    size_t var_count;
    size_t var_capacity;
};

/* Helper: emit a formatted line */
static void emit_line(CodeGen *cg, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(cg->out, fmt, ap);
    va_end(ap);
    fputc('\n', cg->out);
}

/* Manage variable list */
static VarEntry *find_var(CodeGen *cg, const char *name) {
    if (!cg || !name) return NULL;
    for (size_t i = 0; i < cg->var_count; ++i) {
        if (strcmp(cg->vars[i].name, name) == 0) return &cg->vars[i];
    }
    return NULL;
}

static void add_var(CodeGen *cg, const char *name, DataType type, int defined) {
    if (find_var(cg, name)) return; /* already present */
    if (cg->var_count + 1 > cg->var_capacity) {
        size_t nc = cg->var_capacity == 0 ? 8 : cg->var_capacity * 2;
        VarEntry *narr = realloc(cg->vars, nc * sizeof(VarEntry));
        if (!narr) return; /* OOM: best-effort */
        cg->vars = narr;
        cg->var_capacity = nc;
    }
    cg->vars[cg->var_count].name = strdup(name);
    cg->vars[cg->var_count].type = type;
    cg->vars[cg->var_count].defined = defined;
    cg->var_count++;
}

/* Recursively collect identifier usages in an expression/AST */
static void collect_vars_expr(CodeGen *cg, ASTNode *node) {
    if (!node) return;
    switch (node->type) {
        case NODE_ID:
            add_var(cg, node->data.id.name, TYPE_UNKNOWN, 0);
            break;
        case NODE_BINOP:
            collect_vars_expr(cg, node->data.binop.left);
            collect_vars_expr(cg, node->data.binop.right);
            break;
        case NODE_UNOP:
            collect_vars_expr(cg, node->data.unop.operand);
            break;
        default:
            break;
    }
}

static void collect_vars(CodeGen *cg, ASTNode *program) {
    if (!program) return;
    /* Declarations are in program->data.program.declarations */
    ASTNode *decls = program->data.program.declarations;
    if (decls && decls->type == NODE_DECLARATIONS) {
        for (size_t i = 0; i < decls->data.declarations.count; ++i) {
            ASTNode *d = decls->data.declarations.items[i];
            if (d && d->type == NODE_DECLARATION) {
                add_var(cg, d->data.declaration.name, d->data.declaration.type, 1);
            }
        }
    }

    /* Scan commands to find identifier usages (best-effort) */
    ASTNode *cmds = program->data.program.commands;
    if (cmds && cmds->type == NODE_COMMANDS) {
        for (size_t i = 0; i < cmds->data.commands.count; ++i) {
            /* For simplicity, only scan expressions inside commands where applicable */
            ASTNode *c = cmds->data.commands.items[i];
            if (!c) continue;
            switch (c->type) {
                case NODE_ASSIGNMENT:
                    add_var(cg, c->data.assignment.var_name, TYPE_UNKNOWN, 0);
                    collect_vars_expr(cg, c->data.assignment.expr);
                    break;
                case NODE_READ:
                    add_var(cg, c->data.read.var_name, TYPE_UNKNOWN, 0);
                    break;
                case NODE_WRITE:
                case NODE_WRITELN:
                    collect_vars_expr(cg, c->data.write.expr);
                    break;
                case NODE_IF:
                    collect_vars_expr(cg, c->data.if_stmt.condition);
                    /* Not recursing into branches for brevity */
                    break;
                case NODE_WHILE:
                    collect_vars_expr(cg, c->data.while_stmt.condition);
                    break;
                case NODE_FOR:
                    add_var(cg, c->data.for_stmt.var_name, TYPE_UNKNOWN, 0);
                    collect_vars_expr(cg, c->data.for_stmt.start_expr);
                    collect_vars_expr(cg, c->data.for_stmt.end_expr);
                    collect_vars_expr(cg, c->data.for_stmt.step_expr);
                    break;
                default:
                    break;
            }
        }
    }
}

/* Determine whether expression should be compiled as float */
static int expr_is_float(CodeGen *cg, ASTNode *node) {
    if (!node) return 0;
    switch (node->type) {
        case NODE_NUM_FLOAT:
            return 1;
        case NODE_NUM_INT:
            return 0;
        case NODE_ID: {
            VarEntry *v = find_var(cg, node->data.id.name);
            if (v) return v->type == TYPE_FLOAT;
            return 0;
        }
        case NODE_BINOP:
            return expr_is_float(cg, node->data.binop.left) ||
                   expr_is_float(cg, node->data.binop.right);
        case NODE_UNOP:
            return expr_is_float(cg, node->data.unop.operand);
        default:
            return 0;
    }
}

CodeGen *codegen_create(FILE *out) {
    if (!out) return NULL;
    CodeGen *cg = calloc(1, sizeof(CodeGen));
    if (!cg) return NULL;
    cg->out = out;
    cg->label_count = 0;
    cg->vars = NULL;
    cg->var_count = 0;
    cg->var_capacity = 0;
    cg->program = NULL;
    return cg;
}

void codegen_free(CodeGen *cg) {
    if (!cg) return;
    for (size_t i = 0; i < cg->var_count; ++i) {
        free(cg->vars[i].name);
    }
    free(cg->vars);
    free(cg);
}

void codegen_emit_program_cg(CodeGen *cg, ASTNode *program) {
    if (!cg) return;
    cg->program = program;

    /* Collect variables from program (declarations + usages) */
    if (program) collect_vars(cg, program);

    /* Emit data section for known variables */
    emit_line(cg, "; Data section (variables)");
    emit_line(cg, "section .data");
    for (size_t i = 0; i < cg->var_count; ++i) {
        VarEntry *v = &cg->vars[i];
        if (v->type == TYPE_FLOAT) {
            emit_line(cg, "var_%s: dq 0.0", v->name);
        } else {
            /* default to 32-bit integer storage */
            emit_line(cg, "var_%s: dd 0", v->name);
        }
    }

    /* Text section and entry point */
    emit_line(cg, "\nsection .text");
    emit_line(cg, "extern __read_int, __write_int, __write_nl, __read_float, __write_float");
    emit_line(cg, "global main");
    emit_line(cg, "main:");
    emit_line(cg, "    ; function prologue");
    emit_line(cg, "    push ebp");
    emit_line(cg, "    mov ebp, esp");
}

/* Emit expression recursively. Conventions:
 *  - integer results end in EAX
 *  - float results end in ST0
 */
void codegen_emit_expression_cg(CodeGen *cg, ASTNode *expr) {
    if (!cg || !expr) return;

    switch (expr->type) {
        case NODE_NUM_INT: {
            long long v = expr->data.num_int.value;
            emit_line(cg, "    ; integer literal");
            emit_line(cg, "    mov eax, %lld", v);
            return;
        }

        case NODE_NUM_FLOAT: {
            /* Emit constant in .data and load it */
            int id = cg->label_count++;
            emit_line(cg, "    ; float literal");
            emit_line(cg, "section .data");
            emit_line(cg, "const_dbl_%d: dq %.17g", id, expr->data.num_float.value);
            emit_line(cg, "section .text");
            emit_line(cg, "    fld qword [rel const_dbl_%d]", id);
            return;
        }

        case NODE_ID: {
            const char *name = expr->data.id.name;
            VarEntry *v = find_var(cg, name);
            if (!v) {
                /* Create implicit integer variable */
                add_var(cg, name, TYPE_INT, 0);
                /* Emit storage for it */
                emit_line(cg, "section .data");
                emit_line(cg, "var_%s: dd 0", name);
                emit_line(cg, "section .text");
                v = find_var(cg, name);
            }

            if (v->type == TYPE_FLOAT) {
                emit_line(cg, "    fld qword [rel var_%s]", name);
            } else {
                emit_line(cg, "    mov eax, dword [rel var_%s]", name);
            }
            return;
        }

        case NODE_BINOP: {
            char op = expr->data.binop.op;
            ASTNode *L = expr->data.binop.left;
            ASTNode *R = expr->data.binop.right;

            int is_float = expr_is_float(cg, expr);
            if (is_float) {
                /* Float path: push left, push right onto x87 stack then operate */
                codegen_emit_expression_cg(cg, L); /* leaves ST0 = left */
                codegen_emit_expression_cg(cg, R); /* leaves ST0 = right, ST1 = left */

                switch (op) {
                    case '+': emit_line(cg, "    faddp st1, st0"); break;
                    case '-': emit_line(cg, "    fsubp st1, st0"); break;
                    case '*': emit_line(cg, "    fmulp st1, st0"); break;
                    case '/': emit_line(cg, "    fdivp st1, st0"); break;

                    /* Comparisons: use fcomip and setcc */
                    case '=':
                    case '!':
                    case '<':
                    case '>':
                    case '[': /* <= */
                    case ']': /* >= */
                        /* Compare: ST1 (left) vs ST0 (right) */
                        emit_line(cg, "    fcomip st1, st0");
                        emit_line(cg, "    fstp st0");
                        if (op == '=') emit_line(cg, "    sete al");
                        else if (op == '!') emit_line(cg, "    setne al");
                        else if (op == '<') emit_line(cg, "    setl al");
                        else if (op == '>') emit_line(cg, "    setg al");
                        else if (op == '[') emit_line(cg, "    setle al");
                        else if (op == ']') emit_line(cg, "    setge al");
                        emit_line(cg, "    movzx eax, al");
                        break;

                    default:
                        fprintf(stderr, "Unsupported float binary operator '%c'\n", op);
                        break;
                }
                return;
            } else {
                /* Integer path */
                codegen_emit_expression_cg(cg, L); /* EAX = left */
                emit_line(cg, "    push eax");
                codegen_emit_expression_cg(cg, R); /* EAX = right */
                emit_line(cg, "    pop ebx"); /* EBX = left */

                switch (op) {
                    case '+': emit_line(cg, "    add eax, ebx"); break; /* eax = right + left (commutative) */
                    case '-':
                        /* Compute left - right: EBX=left, EAX=right */
                        emit_line(cg, "    mov ecx, eax");
                        emit_line(cg, "    mov eax, ebx");
                        emit_line(cg, "    sub eax, ecx");
                        break;
                    case '*': emit_line(cg, "    imul eax, ebx"); break; /* eax = right * left */
                    case '/':
                        /* left in EBX, right in EAX -> compute EBX / EAX */
                        emit_line(cg, "    mov ecx, eax");
                        emit_line(cg, "    mov eax, ebx");
                        emit_line(cg, "    cdq");
                        emit_line(cg, "    idiv ecx");
                        break;

                    case '=':
                    case '!':
                    case '<':
                    case '>':
                    case '[': /* <= */
                    case ']': /* >= */
                        /* Compare EBX (left) with EAX (right) */
                        emit_line(cg, "    cmp ebx, eax");
                        if (op == '=') emit_line(cg, "    sete al");
                        else if (op == '!') emit_line(cg, "    setne al");
                        else if (op == '<') emit_line(cg, "    setl al");
                        else if (op == '>') emit_line(cg, "    setg al");
                        else if (op == '[') emit_line(cg, "    setle al");
                        else if (op == ']') emit_line(cg, "    setge al");
                        emit_line(cg, "    movzx eax, al");
                        break;

                    default:
                        fprintf(stderr, "Unsupported integer binary operator '%c'\n", op);
                        break;
                }
                return;
            }
        }

        case NODE_UNOP: {
            char op = expr->data.unop.op;
            ASTNode *operand = expr->data.unop.operand;

            int is_float = expr_is_float(cg, operand);
            if (is_float) {
                codegen_emit_expression_cg(cg, operand); /* ST0 = value */
                if (op == '-') {
                    emit_line(cg, "    fchs");
                } else if (op == '~') {
                    /* logical NOT: compare with zero */
                    emit_line(cg, "    fldz"); /* push 0.0 */
                    emit_line(cg, "    fcomip st1, st0");
                    emit_line(cg, "    fstp st0");
                    emit_line(cg, "    sete al");
                    emit_line(cg, "    movzx eax, al");
                } else {
                    fprintf(stderr, "Unsupported float unary operator '%c'\n", op);
                }
            } else {
                codegen_emit_expression_cg(cg, operand); /* EAX = value */
                if (op == '-') {
                    emit_line(cg, "    neg eax");
                } else if (op == '~') {
                    emit_line(cg, "    cmp eax, 0");
                    emit_line(cg, "    sete al");
                    emit_line(cg, "    movzx eax, al");
                } else {
                    fprintf(stderr, "Unsupported integer unary operator '%c'\n", op);
                }
            }
            return;
        }

        default:
            fprintf(stderr, "Unsupported AST node type in codegen: %d\n", expr->type);
            return;
    }
}

/* Generate a new unique label id */
int new_label(CodeGen *cg) {
    if (!cg) return 0;
    return cg->label_count++;
}

/* Emit a single statement (assignments, read, write, if, while, for, commands) */
void codegen_emit_statement_cg(CodeGen *cg, ASTNode *stmt) {
    if (!cg || !stmt) return;

    switch (stmt->type) {
        case NODE_ASSIGNMENT: {
            const char *name = stmt->data.assignment.var_name;
            ASTNode *expr = stmt->data.assignment.expr;
            int is_float = expr_is_float(cg, expr);
            VarEntry *v = find_var(cg, name);
            if (v) {
                if (v->type == TYPE_FLOAT) is_float = 1;
                else if (v->type == TYPE_INT) is_float = 0;
            } else {
                /* create implicit variable with inferred type */
                add_var(cg, name, is_float ? TYPE_FLOAT : TYPE_INT, 0);
                emit_line(cg, "section .data");
                if (is_float) emit_line(cg, "var_%s: dq 0.0", name);
                else emit_line(cg, "var_%s: dd 0", name);
                emit_line(cg, "section .text");
            }

            /* Evaluate right-hand side and store */
            codegen_emit_expression_cg(cg, expr);
            if (is_float) {
                emit_line(cg, "    fstp qword [rel var_%s]", name);
            } else {
                /* store integer result from EAX */
                emit_line(cg, "    mov [rel var_%s], eax", name);
            }
            break;
        }

        case NODE_READ: {
            const char *name = stmt->data.read.var_name;
            VarEntry *v = find_var(cg, name);
            int is_float = 0;
            if (v) is_float = (v->type == TYPE_FLOAT);
            else {
                /* default to int storage */
                add_var(cg, name, TYPE_INT, 0);
                emit_line(cg, "section .data");
                emit_line(cg, "var_%s: dd 0", name);
                emit_line(cg, "section .text");
            }

            if (is_float) {
                emit_line(cg, "    call __read_float");
                emit_line(cg, "    fstp qword [rel var_%s]", name);
            } else {
                emit_line(cg, "    call __read_int");
                emit_line(cg, "    mov [rel var_%s], eax", name);
            }
            break;
        }

        case NODE_WRITE:
        case NODE_WRITELN: {
            ASTNode *expr = stmt->data.write.expr;
            if (expr) {
                int is_float = expr_is_float(cg, expr);
                codegen_emit_expression_cg(cg, expr);
                if (is_float) emit_line(cg, "    call __write_float");
                else emit_line(cg, "    call __write_int");
            }
            if (stmt->type == NODE_WRITELN) {
                emit_line(cg, "    call __write_nl");
            }
            break;
        }

        case NODE_IF: {
            int lbl_else = new_label(cg);
            int lbl_end = new_label(cg);
            codegen_emit_expression_cg(cg, stmt->data.if_stmt.condition);
            emit_line(cg, "    cmp eax, 0");
            emit_line(cg, "    je .L%d", lbl_else);

            ASTNode *thenb = stmt->data.if_stmt.then_branch;
            if (thenb) {
                if (thenb->type == NODE_COMMANDS) codegen_emit_commands_cg(cg, thenb);
                else codegen_emit_statement_cg(cg, thenb);
            }

            emit_line(cg, "    jmp .L%d", lbl_end);
            emit_line(cg, ".L%d:", lbl_else);

            ASTNode *elseb = stmt->data.if_stmt.else_branch;
            if (elseb) {
                if (elseb->type == NODE_COMMANDS) codegen_emit_commands_cg(cg, elseb);
                else codegen_emit_statement_cg(cg, elseb);
            }

            emit_line(cg, ".L%d:", lbl_end);
            break;
        }

        case NODE_WHILE: {
            int lbl_start = new_label(cg);
            int lbl_end = new_label(cg);
            emit_line(cg, ".L%d:", lbl_start);
            codegen_emit_expression_cg(cg, stmt->data.while_stmt.condition);
            emit_line(cg, "    cmp eax, 0");
            emit_line(cg, "    je .L%d", lbl_end);

            ASTNode *body = stmt->data.while_stmt.body;
            if (body) {
                if (body->type == NODE_COMMANDS) codegen_emit_commands_cg(cg, body);
                else codegen_emit_statement_cg(cg, body);
            }

            emit_line(cg, "    jmp .L%d", lbl_start);
            emit_line(cg, ".L%d:", lbl_end);
            break;
        }

        case NODE_FOR: {
            const char *name = stmt->data.for_stmt.var_name;
            ASTNode *start = stmt->data.for_stmt.start_expr;
            ASTNode *end = stmt->data.for_stmt.end_expr;
            ASTNode *step = stmt->data.for_stmt.step_expr;
            ASTNode *body = stmt->data.for_stmt.body;

            VarEntry *v = find_var(cg, name);
            if (!v) {
                add_var(cg, name, TYPE_INT, 0);
                emit_line(cg, "section .data");
                emit_line(cg, "var_%s: dd 0", name);
                emit_line(cg, "section .text");
            }

            /* Initialize loop variable */
            codegen_emit_expression_cg(cg, start);
            emit_line(cg, "    mov [rel var_%s], eax", name);

            /* Prepare end and step temporaries */
            char endname[128];
            char stepname[128];
            snprintf(endname, sizeof(endname), "var_%s_end", name);
            snprintf(stepname, sizeof(stepname), "var_%s_step", name);
            emit_line(cg, "section .data");
            emit_line(cg, "%s: dd 0", endname);
            emit_line(cg, "%s: dd 0", stepname);
            emit_line(cg, "section .text");

            /* store end */
            codegen_emit_expression_cg(cg, end);
            emit_line(cg, "    mov [rel %s], eax", endname);

            /* store step (default 1) */
            if (step) {
                codegen_emit_expression_cg(cg, step);
                emit_line(cg, "    mov [rel %s], eax", stepname);
            } else {
                emit_line(cg, "    mov eax, 1");
                emit_line(cg, "    mov [rel %s], eax", stepname);
            }

            int lbl_start = new_label(cg);
            int lbl_end = new_label(cg);
            emit_line(cg, ".L%d:", lbl_start);
            emit_line(cg, "    mov eax, dword [rel var_%s]", name);
            emit_line(cg, "    mov ecx, dword [rel %s]", endname);
            emit_line(cg, "    cmp eax, ecx");
            emit_line(cg, "    jg .L%d", lbl_end);

            if (body) {
                if (body->type == NODE_COMMANDS) codegen_emit_commands_cg(cg, body);
                else codegen_emit_statement_cg(cg, body);
            }

            emit_line(cg, "    mov eax, dword [rel var_%s]", name);
            emit_line(cg, "    add eax, dword [rel %s]", stepname);
            emit_line(cg, "    mov [rel var_%s], eax", name);
            emit_line(cg, "    jmp .L%d", lbl_start);
            emit_line(cg, ".L%d:", lbl_end);
            break;
        }

        case NODE_COMMANDS:
            codegen_emit_commands_cg(cg, stmt);
            break;

        default:
            /* Unsupported statement */
            break;
    }
}


void codegen_emit_commands_cg(CodeGen *cg, ASTNode *commands) {
    if (!cg || !commands) return;
    if (commands->type != NODE_COMMANDS) return;
    for (size_t i = 0; i < commands->data.commands.count; ++i) {
        ASTNode *c = commands->data.commands.items[i];
        if (!c) continue;
        codegen_emit_statement_cg(cg, c);
    }
}

void codegen_finish_cg(CodeGen *cg) {
    if (!cg) return;
    emit_line(cg, "\n    ; function epilogue");
    emit_line(cg, "    mov esp, ebp");
    emit_line(cg, "    pop ebp");
    emit_line(cg, "    ret");
    fflush(cg->out);
}

/* --- Global wrapper API --- */
static CodeGen *global_cg = NULL;

static void ensure_parent_dir_exists(const char *path) {
    if (!path) return;
    const char *sep1 = strrchr(path, '/');
    const char *sep2 = strrchr(path, '\\');
    const char *sep = sep1 > sep2 ? sep1 : sep2;
    if (!sep) return; /* no parent dir */
    size_t len = sep - path;
    if (len == 0) return;
    char dir[1024];
    if (len >= sizeof(dir)) return;
    memcpy(dir, path, len);
    dir[len] = '\0';

    struct stat st;
    if (stat(dir, &st) == 0) return; /* exists */

#ifdef _WIN32
    _mkdir(dir);
#else
    mkdir(dir, 0755);
#endif
}

void codegen_init(const char *out_asm_path) {
    if (global_cg) return; /* already initialized */
    if (!out_asm_path) return;
    ensure_parent_dir_exists(out_asm_path);
    FILE *out = fopen(out_asm_path, "w");
    if (!out) {
        perror("fopen");
        return;
    }
    global_cg = codegen_create(out);
}

void codegen_emit_program(ASTNode *program) {
    if (!global_cg) return;
    codegen_emit_program_cg(global_cg, program);
}

void codegen_emit_expression(ASTNode *expr) {
    if (!global_cg) return;
    codegen_emit_expression_cg(global_cg, expr);
}

void codegen_emit_statement(ASTNode *stmt) {
    if (!global_cg) return;
    codegen_emit_statement_cg(global_cg, stmt);
}

void codegen_emit_commands(ASTNode *commands) {
    if (!global_cg) return;
    codegen_emit_commands_cg(global_cg, commands);
}

void codegen_finish(void) {
    if (!global_cg) return;
    codegen_finish_cg(global_cg);
    if (global_cg->out) fclose(global_cg->out);
    codegen_free(global_cg);
    global_cg = NULL;
}
