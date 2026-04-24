#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

/* ================================================================
 * Code Generator: NASM x86-32 (ELF Linux) com syscalls int 0x80
 * Conforme especificado no PRD do compilador SIMPLES.
 *
 * Convenções:
 *   - Resultados inteiros ficam em EAX
 *   - Variáveis são armazenadas na seção .bss (resd 1)
 *   - I/O via sys_read (3) e sys_write (4) + conversão ASCII
 *   - Exit via sys_exit (1)
 *   - Labels únicos: .L<N> para controle de fluxo
 * ================================================================ */

/* ---- Variable tracking ---- */
typedef struct VarEntry {
    char *name;
    DataType type;
} VarEntry;

typedef struct CodeGen {
    FILE *out;
    int label_count;
    VarEntry *vars;
    size_t var_count;
    size_t var_capacity;
    int needs_buf;        /* se precisa do buffer de I/O */
    int needs_read_int;   /* se precisa da rotina _read_int */
    int needs_write_int;  /* se precisa da rotina _write_int */
    int needs_write_nl;   /* se precisa da rotina _write_nl */
} CodeGen;

/* ---- Forward declarations ---- */
static void emit(CodeGen *cg, const char *fmt, ...);
static void emit_expression(CodeGen *cg, ASTNode *expr);
static void emit_statement(CodeGen *cg, ASTNode *stmt);
static void emit_commands(CodeGen *cg, ASTNode *commands);
static int new_label(CodeGen *cg);
static VarEntry *find_var(CodeGen *cg, const char *name);
static void add_var(CodeGen *cg, const char *name, DataType type);
static void collect_all_vars(CodeGen *cg, ASTNode *node);

/* ---- Emit helper ---- */
static void emit(CodeGen *cg, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(cg->out, fmt, ap);
    va_end(ap);
    fputc('\n', cg->out);
}

/* ---- Label counter ---- */
static int new_label(CodeGen *cg) {
    return cg->label_count++;
}

/* ---- Variable management ---- */
static VarEntry *find_var(CodeGen *cg, const char *name) {
    if (!cg || !name) return NULL;
    for (size_t i = 0; i < cg->var_count; i++) {
        if (strcmp(cg->vars[i].name, name) == 0) return &cg->vars[i];
    }
    return NULL;
}

static void add_var(CodeGen *cg, const char *name, DataType type) {
    if (find_var(cg, name)) return;
    if (cg->var_count >= cg->var_capacity) {
        size_t nc = cg->var_capacity == 0 ? 16 : cg->var_capacity * 2;
        cg->vars = realloc(cg->vars, nc * sizeof(VarEntry));
        cg->var_capacity = nc;
    }
    cg->vars[cg->var_count].name = strdup(name);
    cg->vars[cg->var_count].type = type;
    cg->var_count++;
}

/* ---- Recursively collect all variables from the AST ---- */
static void collect_all_vars(CodeGen *cg, ASTNode *node) {
    if (!node) return;
    switch (node->type) {
        case NODE_PROGRAM:
            collect_all_vars(cg, node->data.program.declarations);
            collect_all_vars(cg, node->data.program.commands);
            break;
        case NODE_DECLARATIONS:
            for (size_t i = 0; i < node->data.declarations.count; i++)
                collect_all_vars(cg, node->data.declarations.items[i]);
            break;
        case NODE_DECLARATION:
            add_var(cg, node->data.declaration.name, node->data.declaration.type);
            break;
        case NODE_COMMANDS:
            for (size_t i = 0; i < node->data.commands.count; i++)
                collect_all_vars(cg, node->data.commands.items[i]);
            break;
        case NODE_ASSIGNMENT:
            add_var(cg, node->data.assignment.var_name, TYPE_UNKNOWN);
            collect_all_vars(cg, node->data.assignment.expr);
            break;
        case NODE_READ:
            add_var(cg, node->data.read.var_name, TYPE_UNKNOWN);
            cg->needs_read_int = 1;
            cg->needs_buf = 1;
            break;
        case NODE_WRITE:
            collect_all_vars(cg, node->data.write.expr);
            cg->needs_write_int = 1;
            cg->needs_buf = 1;
            break;
        case NODE_WRITELN:
            collect_all_vars(cg, node->data.write.expr);
            cg->needs_write_int = 1;
            cg->needs_write_nl = 1;
            cg->needs_buf = 1;
            break;
        case NODE_IF:
            collect_all_vars(cg, node->data.if_stmt.condition);
            collect_all_vars(cg, node->data.if_stmt.then_branch);
            collect_all_vars(cg, node->data.if_stmt.else_branch);
            break;
        case NODE_WHILE:
            collect_all_vars(cg, node->data.while_stmt.condition);
            collect_all_vars(cg, node->data.while_stmt.body);
            break;
        case NODE_FOR:
            add_var(cg, node->data.for_stmt.var_name, TYPE_UNKNOWN);
            collect_all_vars(cg, node->data.for_stmt.start_expr);
            collect_all_vars(cg, node->data.for_stmt.end_expr);
            collect_all_vars(cg, node->data.for_stmt.step_expr);
            collect_all_vars(cg, node->data.for_stmt.body);
            break;
        case NODE_BINOP:
            collect_all_vars(cg, node->data.binop.left);
            collect_all_vars(cg, node->data.binop.right);
            break;
        case NODE_UNOP:
            collect_all_vars(cg, node->data.unop.operand);
            break;
        case NODE_ID:
            add_var(cg, node->data.id.name, TYPE_UNKNOWN);
            break;
        default:
            break;
    }
}

/* ================================================================
 * Expression code generation
 * Result always ends in EAX.
 * Uses the stack (push/pop) for subexpressions.
 * ================================================================ */
static void emit_expression(CodeGen *cg, ASTNode *expr) {
    if (!cg || !expr) return;

    switch (expr->type) {
        case NODE_NUM_INT:
            emit(cg, "    mov eax, %lld", expr->data.num_int.value);
            break;

        case NODE_NUM_FLOAT:
            /* Para v1.0, tratar float como inteiro truncado */
            emit(cg, "    mov eax, %d", (int)expr->data.num_float.value);
            break;

        case NODE_ID:
            emit(cg, "    mov eax, [%s]", expr->data.id.name);
            break;

        case NODE_BINOP: {
            char op = expr->data.binop.op;
            /* Avaliar lado esquerdo -> EAX -> push na pilha */
            emit_expression(cg, expr->data.binop.left);
            emit(cg, "    push eax");
            /* Avaliar lado direito -> EAX */
            emit_expression(cg, expr->data.binop.right);
            /* Pop esquerdo para EBX */
            emit(cg, "    pop ebx");
            /* EBX = left, EAX = right */

            switch (op) {
                case '+':
                    emit(cg, "    add eax, ebx");
                    break;
                case '-':
                    /* left - right: resultado em EAX */
                    emit(cg, "    sub ebx, eax");
                    emit(cg, "    mov eax, ebx");
                    break;
                case '*':
                    emit(cg, "    imul eax, ebx");
                    break;
                case '/':
                    /* left / right: EBX / EAX */
                    emit(cg, "    mov ecx, eax");
                    emit(cg, "    mov eax, ebx");
                    emit(cg, "    cdq");
                    emit(cg, "    idiv ecx");
                    break;

                /* Operadores relacionais: resultado 0 ou 1 em EAX */
                case '=':
                    emit(cg, "    cmp ebx, eax");
                    emit(cg, "    sete al");
                    emit(cg, "    movzx eax, al");
                    break;
                case '!': /* <> */
                    emit(cg, "    cmp ebx, eax");
                    emit(cg, "    setne al");
                    emit(cg, "    movzx eax, al");
                    break;
                case '<':
                    emit(cg, "    cmp ebx, eax");
                    emit(cg, "    setl al");
                    emit(cg, "    movzx eax, al");
                    break;
                case '>':
                    emit(cg, "    cmp ebx, eax");
                    emit(cg, "    setg al");
                    emit(cg, "    movzx eax, al");
                    break;
                case '[': /* <= */
                    emit(cg, "    cmp ebx, eax");
                    emit(cg, "    setle al");
                    emit(cg, "    movzx eax, al");
                    break;
                case ']': /* >= */
                    emit(cg, "    cmp ebx, eax");
                    emit(cg, "    setge al");
                    emit(cg, "    movzx eax, al");
                    break;

                /* Operadores lógicos */
                case '&': /* e (AND) */
                    emit(cg, "    and eax, ebx");
                    break;
                case '|': /* ou (OR) */
                    emit(cg, "    or eax, ebx");
                    break;

                default:
                    emit(cg, "    ; operador desconhecido '%c'", op);
                    break;
            }
            break;
        }

        case NODE_UNOP: {
            char op = expr->data.unop.op;
            emit_expression(cg, expr->data.unop.operand);
            if (op == '-') {
                emit(cg, "    neg eax");
            } else if (op == '~') { /* nao */
                emit(cg, "    cmp eax, 0");
                emit(cg, "    sete al");
                emit(cg, "    movzx eax, al");
            }
            break;
        }

        default:
            emit(cg, "    ; expressao nao suportada (tipo %d)", expr->type);
            break;
    }
}

/* ================================================================
 * Statement code generation
 * ================================================================ */
static void emit_statement(CodeGen *cg, ASTNode *stmt) {
    if (!cg || !stmt) return;

    switch (stmt->type) {
        case NODE_ASSIGNMENT: {
            const char *name = stmt->data.assignment.var_name;
            emit(cg, "");
            emit(cg, "    ; %s <- ...", name);
            emit_expression(cg, stmt->data.assignment.expr);
            emit(cg, "    mov [%s], eax", name);
            break;
        }

        case NODE_READ: {
            const char *name = stmt->data.read.var_name;
            emit(cg, "");
            emit(cg, "    ; leia %s", name);
            /* sys_read(stdin, buf, 12) */
            emit(cg, "    mov eax, 3");
            emit(cg, "    mov ebx, 0");
            emit(cg, "    mov ecx, _buf");
            emit(cg, "    mov edx, 12");
            emit(cg, "    int 0x80");
            /* Converter ASCII -> inteiro */
            emit(cg, "    ; converter ASCII -> inteiro");
            emit(cg, "    xor eax, eax");
            emit(cg, "    xor ecx, ecx");
            emit(cg, "    mov esi, _buf");
            /* Handle optional negative sign */
            emit(cg, "    xor edi, edi          ; edi=0 means positive");
            emit(cg, "    cmp byte [esi], '-'");
            int lbl_pos = new_label(cg);
            emit(cg, "    jne .L%d", lbl_pos);
            emit(cg, "    mov edi, 1");
            emit(cg, "    inc esi");
            emit(cg, ".L%d:", lbl_pos);
            int lbl_loop = new_label(cg);
            int lbl_done = new_label(cg);
            emit(cg, ".L%d:", lbl_loop);
            emit(cg, "    movzx ecx, byte [esi]");
            emit(cg, "    cmp ecx, 10");
            emit(cg, "    je .L%d", lbl_done);
            emit(cg, "    cmp ecx, 13");
            emit(cg, "    je .L%d", lbl_done);
            emit(cg, "    cmp ecx, 0");
            emit(cg, "    je .L%d", lbl_done);
            emit(cg, "    sub ecx, 48");
            emit(cg, "    imul eax, 10");
            emit(cg, "    add eax, ecx");
            emit(cg, "    inc esi");
            emit(cg, "    jmp .L%d", lbl_loop);
            emit(cg, ".L%d:", lbl_done);
            /* Negate if needed */
            int lbl_store = new_label(cg);
            emit(cg, "    cmp edi, 0");
            emit(cg, "    je .L%d", lbl_store);
            emit(cg, "    neg eax");
            emit(cg, ".L%d:", lbl_store);
            emit(cg, "    mov [%s], eax", name);
            break;
        }

        case NODE_WRITE:
        case NODE_WRITELN: {
            emit(cg, "");
            emit(cg, "    ; escreva%s ...",
                 stmt->type == NODE_WRITELN ? "l" : "");
            emit_expression(cg, stmt->data.write.expr);
            /* Converter inteiro (EAX) -> ASCII e escrever via sys_write */
            /* Tratamento de número negativo */
            int lbl_positive = new_label(cg);
            emit(cg, "    ; converter inteiro -> ASCII");
            emit(cg, "    mov edi, 0           ; edi=0 not negative");
            emit(cg, "    cmp eax, 0");
            emit(cg, "    jge .L%d", lbl_positive);
            emit(cg, "    neg eax");
            emit(cg, "    mov edi, 1           ; negative flag");
            emit(cg, ".L%d:", lbl_positive);
            emit(cg, "    mov ecx, _buf + 11");
            emit(cg, "    mov byte [ecx], 0    ; null terminator");
            emit(cg, "    dec ecx");
            int lbl_wloop = new_label(cg);
            emit(cg, ".L%d:", lbl_wloop);
            emit(cg, "    xor edx, edx");
            emit(cg, "    mov ebx, 10");
            emit(cg, "    div ebx");
            emit(cg, "    add dl, 48");
            emit(cg, "    mov [ecx], dl");
            emit(cg, "    dec ecx");
            emit(cg, "    test eax, eax");
            emit(cg, "    jnz .L%d", lbl_wloop);
            /* Add '-' if negative */
            int lbl_write = new_label(cg);
            emit(cg, "    cmp edi, 0");
            emit(cg, "    je .L%d", lbl_write);
            emit(cg, "    mov byte [ecx], '-'");
            emit(cg, "    dec ecx");
            emit(cg, ".L%d:", lbl_write);
            emit(cg, "    inc ecx");
            /* Calcular comprimento */
            emit(cg, "    mov edx, _buf + 11");
            emit(cg, "    sub edx, ecx");
            /* Adicionar newline se escreval */
            if (stmt->type == NODE_WRITELN) {
                emit(cg, "    mov esi, ecx          ; salvar inicio");
                emit(cg, "    mov eax, ecx");
                emit(cg, "    add eax, edx");
                emit(cg, "    mov byte [eax], 10    ; newline");
                emit(cg, "    inc edx");
                emit(cg, "    mov ecx, esi          ; restaurar inicio");
            }
            /* sys_write(stdout, ecx, edx) */
            emit(cg, "    mov eax, 4");
            emit(cg, "    mov ebx, 1");
            emit(cg, "    int 0x80");
            break;
        }

        case NODE_IF: {
            int lbl_else = new_label(cg);
            int lbl_end = new_label(cg);
            emit(cg, "");
            emit(cg, "    ; se ... entao");
            emit_expression(cg, stmt->data.if_stmt.condition);
            emit(cg, "    cmp eax, 0");
            emit(cg, "    je .L%d", lbl_else);

            /* Bloco entao */
            if (stmt->data.if_stmt.then_branch) {
                if (stmt->data.if_stmt.then_branch->type == NODE_COMMANDS)
                    emit_commands(cg, stmt->data.if_stmt.then_branch);
                else
                    emit_statement(cg, stmt->data.if_stmt.then_branch);
            }

            emit(cg, "    jmp .L%d", lbl_end);
            emit(cg, ".L%d:", lbl_else);

            /* Bloco senao (opcional) */
            if (stmt->data.if_stmt.else_branch) {
                emit(cg, "    ; senao");
                if (stmt->data.if_stmt.else_branch->type == NODE_COMMANDS)
                    emit_commands(cg, stmt->data.if_stmt.else_branch);
                else
                    emit_statement(cg, stmt->data.if_stmt.else_branch);
            }

            emit(cg, ".L%d:", lbl_end);
            emit(cg, "    ; fimse");
            break;
        }

        case NODE_WHILE: {
            int lbl_start = new_label(cg);
            int lbl_end = new_label(cg);
            emit(cg, "");
            emit(cg, "    ; enquanto ... faca");
            emit(cg, ".L%d:", lbl_start);
            emit_expression(cg, stmt->data.while_stmt.condition);
            emit(cg, "    cmp eax, 0");
            emit(cg, "    je .L%d", lbl_end);

            if (stmt->data.while_stmt.body) {
                if (stmt->data.while_stmt.body->type == NODE_COMMANDS)
                    emit_commands(cg, stmt->data.while_stmt.body);
                else
                    emit_statement(cg, stmt->data.while_stmt.body);
            }

            emit(cg, "    jmp .L%d", lbl_start);
            emit(cg, ".L%d:", lbl_end);
            emit(cg, "    ; fimenquanto");
            break;
        }

        case NODE_FOR: {
            const char *name = stmt->data.for_stmt.var_name;
            int lbl_start = new_label(cg);
            int lbl_end = new_label(cg);

            emit(cg, "");
            emit(cg, "    ; para %s de ... ate ... passo ... faca", name);

            /* Inicializar variável de loop */
            emit_expression(cg, stmt->data.for_stmt.start_expr);
            emit(cg, "    mov [%s], eax", name);

            /* Avaliar e guardar o valor final */
            emit_expression(cg, stmt->data.for_stmt.end_expr);
            emit(cg, "    push eax              ; guardar fim na pilha");

            /* Avaliar e guardar o passo */
            if (stmt->data.for_stmt.step_expr) {
                emit_expression(cg, stmt->data.for_stmt.step_expr);
            } else {
                emit(cg, "    mov eax, 1");
            }
            emit(cg, "    push eax              ; guardar passo na pilha");

            emit(cg, ".L%d:", lbl_start);
            /* Comparar: var > fim? -> sai */
            emit(cg, "    mov eax, [%s]", name);
            emit(cg, "    mov ecx, [esp+4]      ; ecx = fim");
            emit(cg, "    cmp eax, ecx");
            emit(cg, "    jg .L%d", lbl_end);

            /* Corpo do loop */
            if (stmt->data.for_stmt.body) {
                if (stmt->data.for_stmt.body->type == NODE_COMMANDS)
                    emit_commands(cg, stmt->data.for_stmt.body);
                else
                    emit_statement(cg, stmt->data.for_stmt.body);
            }

            /* Incrementar variável pelo passo */
            emit(cg, "    mov eax, [%s]", name);
            emit(cg, "    add eax, [esp]        ; eax += passo");
            emit(cg, "    mov [%s], eax", name);
            emit(cg, "    jmp .L%d", lbl_start);

            emit(cg, ".L%d:", lbl_end);
            emit(cg, "    add esp, 8            ; limpar pilha (fim + passo)");
            emit(cg, "    ; fimpara");
            break;
        }

        case NODE_COMMANDS:
            emit_commands(cg, stmt);
            break;

        default:
            emit(cg, "    ; statement nao suportado (tipo %d)", stmt->type);
            break;
    }
}

/* ---- Emit all commands in a commands node ---- */
static void emit_commands(CodeGen *cg, ASTNode *commands) {
    if (!cg || !commands || commands->type != NODE_COMMANDS) return;
    for (size_t i = 0; i < commands->data.commands.count; i++) {
        if (commands->data.commands.items[i])
            emit_statement(cg, commands->data.commands.items[i]);
    }
}

/* ================================================================
 * Program-level emission
 * ================================================================ */
static void emit_program(CodeGen *cg, ASTNode *program) {
    if (!cg || !program || program->type != NODE_PROGRAM) return;

    /* 1. Coletar todas as variáveis */
    collect_all_vars(cg, program);

    /* 2. Emitir seção .bss (variáveis não inicializadas) */
    emit(cg, "; Compilador SIMPLES -> NASM x86-32");
    emit(cg, "; Programa: %s", program->data.program.name ?
         program->data.program.name : "sem_nome");
    emit(cg, "");
    emit(cg, "section .bss");
    for (size_t i = 0; i < cg->var_count; i++) {
        emit(cg, "    %s resd 1", cg->vars[i].name);
    }
    if (cg->needs_buf) {
        emit(cg, "    _buf resb 12");
    }

    /* 3. Emitir seção .text */
    emit(cg, "");
    emit(cg, "section .text");
    emit(cg, "    global _start");
    emit(cg, "");
    emit(cg, "_start:");

    /* 4. Emitir comandos */
    if (program->data.program.commands) {
        emit_commands(cg, program->data.program.commands);
    }

    /* 5. Epílogo: sys_exit(0) */
    emit(cg, "");
    emit(cg, "    ; exit(0)");
    emit(cg, "    mov eax, 1");
    emit(cg, "    xor ebx, ebx");
    emit(cg, "    int 0x80");
}

/* ================================================================
 * Global API — wraps the internal CodeGen struct
 * ================================================================ */
static CodeGen *global_cg = NULL;

static void ensure_parent_dir(const char *path) {
    if (!path) return;
    const char *sep1 = strrchr(path, '/');
    const char *sep2 = strrchr(path, '\\');
    const char *sep = sep1 > sep2 ? sep1 : sep2;
    if (!sep || sep == path) return;
    size_t len = (size_t)(sep - path);
    char *dir = malloc(len + 1);
    if (!dir) return;
    memcpy(dir, path, len);
    dir[len] = '\0';
    struct stat st;
    if (stat(dir, &st) != 0) {
#ifdef _WIN32
        _mkdir(dir);
#else
        mkdir(dir, 0755);
#endif
    }
    free(dir);
}

void codegen_init(const char *out_asm_path) {
    if (global_cg) return;
    if (!out_asm_path) return;
    ensure_parent_dir(out_asm_path);
    FILE *out = fopen(out_asm_path, "w");
    if (!out) { perror("fopen"); return; }

    global_cg = calloc(1, sizeof(CodeGen));
    if (!global_cg) { fclose(out); return; }
    global_cg->out = out;
}

void codegen_emit_program(ASTNode *program) {
    if (!global_cg) return;
    emit_program(global_cg, program);
}

void codegen_emit_expression(ASTNode *expr) {
    if (!global_cg) return;
    emit_expression(global_cg, expr);
}

void codegen_emit_statement(ASTNode *stmt) {
    if (!global_cg) return;
    emit_statement(global_cg, stmt);
}

void codegen_emit_commands(ASTNode *commands) {
    if (!global_cg) return;
    emit_commands(global_cg, commands);
}

void codegen_finish(void) {
    if (!global_cg) return;
    fflush(global_cg->out);
    if (global_cg->out) fclose(global_cg->out);
    for (size_t i = 0; i < global_cg->var_count; i++)
        free(global_cg->vars[i].name);
    free(global_cg->vars);
    free(global_cg);
    global_cg = NULL;
}
