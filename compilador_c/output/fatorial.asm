; Compilador SIMPLES -> NASM x86-32
; Programa: fatorial

section .bss
    n resd 1
    fat resd 1
    contador resd 1
    _buf resb 12

section .text
    global _start

_start:

    ; leia n
    mov eax, 3
    mov ebx, 0
    mov ecx, _buf
    mov edx, 12
    int 0x80
    ; converter ASCII -> inteiro
    xor eax, eax
    xor ecx, ecx
    mov esi, _buf
    xor edi, edi          ; edi=0 means positive
    cmp byte [esi], '-'
    jne .L0
    mov edi, 1
    inc esi
.L0:
.L1:
    movzx ecx, byte [esi]
    cmp ecx, 10
    je .L2
    cmp ecx, 13
    je .L2
    cmp ecx, 0
    je .L2
    sub ecx, 48
    imul eax, 10
    add eax, ecx
    inc esi
    jmp .L1
.L2:
    cmp edi, 0
    je .L3
    neg eax
.L3:
    mov [n], eax

    ; fat <- ...
    mov eax, 1
    mov [fat], eax

    ; contador <- ...
    mov eax, 1
    mov [contador], eax

    ; enquanto ... faca
.L4:
    mov eax, [contador]
    push eax
    mov eax, [n]
    pop ebx
    cmp ebx, eax
    setl al
    movzx eax, al
    cmp eax, 0
    je .L5

    ; contador <- ...
    mov eax, [contador]
    push eax
    mov eax, 1
    pop ebx
    add eax, ebx
    mov [contador], eax

    ; fat <- ...
    mov eax, [fat]
    push eax
    mov eax, [contador]
    pop ebx
    imul eax, ebx
    mov [fat], eax
    jmp .L4
.L5:
    ; fimenquanto

    ; escreval ...
    mov eax, [fat]
    ; converter inteiro -> ASCII
    mov edi, 0           ; edi=0 not negative
    cmp eax, 0
    jge .L6
    neg eax
    mov edi, 1           ; negative flag
.L6:
    mov ecx, _buf + 11
    mov byte [ecx], 0    ; null terminator
    dec ecx
.L7:
    xor edx, edx
    mov ebx, 10
    div ebx
    add dl, 48
    mov [ecx], dl
    dec ecx
    test eax, eax
    jnz .L7
    cmp edi, 0
    je .L8
    mov byte [ecx], '-'
    dec ecx
.L8:
    inc ecx
    mov edx, _buf + 11
    sub edx, ecx
    mov esi, ecx          ; salvar inicio
    mov eax, ecx
    add eax, edx
    mov byte [eax], 10    ; newline
    inc edx
    mov ecx, esi          ; restaurar inicio
    mov eax, 4
    mov ebx, 1
    int 0x80

    ; exit(0)
    mov eax, 1
    xor ebx, ebx
    int 0x80
