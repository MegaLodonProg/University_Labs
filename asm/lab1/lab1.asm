section .data

; db    Define Byte  char       1 байт
; dw    Define Word  short      2 байта
; dd    Define Double word      4 байта
; dq    Define Quad word        8 байт

a: dq 5
b: dd -3
c: dd 10
d: db -2
e: dw 4
result: dq 0 ; место для итога

section .text
    global _start

_start:
    ; (a * c) / b
    mov rax, [a]
    movsx rcx, dword [c] ; 32 -> 64
    imul rcx
    jo mul_overflow_error ; если результат не влез полностью в rax в 64 бит -> установился флаг OF

    movsx rbx, dword [b] ; расширяем до размера младшей части делимого (RAX), чтобы взялась часть RDX:RAX, а не EDX:EAX с обрезкой
    cmp rbx, 0
    je division_by_zero ; Jump if Equal cmp rbx, 0  je проверяет  ZF=1

    idiv rbx; после деления : RAX содержит частное, RDX - остаток (не нужен нам)
    mov r8, rax ; сохранение промежуточного слагаемого в другой регистр

    ; (d * b) / e
    movsx rax, byte [d] ; 8 -> 64, теперь результат произведения точно влезет в 64 бита
    imul rbx ; b лежит по-прежнему в rbx,
    movsx rbx, word [e] ; b - больше не нужна, могу перезаписать туда значение для e
    cmp rbx, 0
    je division_by_zero

    idiv rbx
    jo division_overflow_error ; проверка деления (для -1)

    add r8, rax
    jo add_overflow_error

    ; (c ^ 2) / (a * d)
    mov rax, rcx ; копируем значение c в регистр rax
    imul rcx ; c лежит по-прежнему в rcx, результат точно влезает в 64 бита (с^2 <= 64 бита)
    mov rcx, [a] ; переменная "с" больше не нужна
    cmp rcx, 0
    je division_by_zero
    idiv rcx
    jo division_overflow_error ; проверка деления (для -1)

    movsx rcx, byte [d]
    cmp rcx, 0
    je division_by_zero
    cqo
    idiv rcx

    sub r8, rax ; вычитаем последнее слагаемое
    jo sub_overflow_error
    mov qword [result], r8

    ; Завершение программы
exit:
    mov eax, 60
    syscall


division_by_zero:
    mov edi, 1
    jmp exit

division_overflow_error:
    mov edi, 2
    jmp exit

mul_overflow_error:
    mov edi, 3
    jmp exit


add_overflow_error:
    mov edi, 4
    jmp exit

sub_overflow_error:
    mov edi, 5
    jmp exit
