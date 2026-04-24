; Compilador SIMPLES -> NASM x86-32
; Programa: fibonacci

section .bss
    n resd 1
    a resd 1
    b resd 1
    temp resd 1
    i resd 1
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

    ; a <- ...
    mov eax, 0
    mov [a], eax

    ; b <- ...
    mov eax, 1
    mov [b], eax

    ; se ... entao
    mov eax, [n]
    push eax
    mov eax, 0
    pop ebx
    cmp ebx, eax
    sete al
    movzx eax, al
    cmp eax, 0
    je .L4

    ; escreval ...
    mov eax, [a]
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
    jmp .L5
.L4:
    ; senao

    ; se ... entao
    mov eax, [n]
    push eax
    mov eax, 1
    pop ebx
    cmp ebx, eax
    sete al
    movzx eax, al
    cmp eax, 0
    je .L9

    ; escreval ...
    mov eax, [b]
    ; converter inteiro -> ASCII
    mov edi, 0           ; edi=0 not negative
    cmp eax, 0
    jge .L11
    neg eax
    mov edi, 1           ; negative flag
.L11:
    mov ecx, _buf + 11
    mov byte [ecx], 0    ; null terminator
    dec ecx
.L12:
    xor edx, edx
    mov ebx, 10
    div ebx
    add dl, 48
    mov [ecx], dl
    dec ecx
    test eax, eax
    jnz .L12
    cmp edi, 0
    je .L13
    mov byte [ecx], '-'
    dec ecx
.L13:
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
    jmp .L10
.L9:
    ; senao

    ; para i de ... ate ... passo ... faca
    mov eax, 2
    mov [i], eax
    mov eax, [n]
    push eax              ; guardar fim na pilha
    mov eax, 1
    push eax              ; guardar passo na pilha
.L14:
    mov eax, [i]
    mov ecx, [esp+4]      ; ecx = fim
    cmp eax, ecx
    jg .L15

    ; temp <- ...
    mov eax, [a]
    push eax
    mov eax, [b]
    pop ebx
    add eax, ebx
    mov [temp], eax

    ; a <- ...
    mov eax, [b]
    mov [a], eax

    ; b <- ...
    mov eax, [temp]
    mov [b], eax
    mov eax, [i]
    add eax, [esp]        ; eax += passo
    mov [i], eax
    jmp .L14
.L15:
    add esp, 8            ; limpar pilha (fim + passo)
    ; fimpara

    ; escreval ...
    mov eax, [b]
    ; converter inteiro -> ASCII
    mov edi, 0           ; edi=0 not negative
    cmp eax, 0
    jge .L16
    neg eax
    mov edi, 1           ; negative flag
.L16:
    mov ecx, _buf + 11
    mov byte [ecx], 0    ; null terminator
    dec ecx
.L17:
    xor edx, edx
    mov ebx, 10
    div ebx
    add dl, 48
    mov [ecx], dl
    dec ecx
    test eax, eax
    jnz .L17
    cmp edi, 0
    je .L18
    mov byte [ecx], '-'
    dec ecx
.L18:
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
.L10:
    ; fimse
.L5:
    ; fimse

    ; exit(0)
    mov eax, 1
    xor ebx, ebx
    int 0x80
