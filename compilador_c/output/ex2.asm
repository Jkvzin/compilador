; Compilador SIMPLES -> NASM x86-32
; Programa: Soma

section .bss
    a resd 1
    b resd 1
    s resd 1
    _buf resb 12

section .text
    global _start

_start:

    ; a <- ...
    mov eax, 2
    mov [a], eax

    ; b <- ...
    mov eax, 3
    mov [b], eax

    ; s <- ...
    mov eax, [a]
    push eax
    mov eax, [b]
    pop ebx
    add eax, ebx
    mov [s], eax

    ; escreva ...
    mov eax, [s]
    ; converter inteiro -> ASCII
    mov edi, 0           ; edi=0 not negative
    cmp eax, 0
    jge .L0
    neg eax
    mov edi, 1           ; negative flag
.L0:
    mov ecx, _buf + 11
    mov byte [ecx], 0    ; null terminator
    dec ecx
.L1:
    xor edx, edx
    mov ebx, 10
    div ebx
    add dl, 48
    mov [ecx], dl
    dec ecx
    test eax, eax
    jnz .L1
    cmp edi, 0
    je .L2
    mov byte [ecx], '-'
    dec ecx
.L2:
    inc ecx
    mov edx, _buf + 11
    sub edx, ecx
    mov eax, 4
    mov ebx, 1
    int 0x80

    ; exit(0)
    mov eax, 1
    xor ebx, ebx
    int 0x80
