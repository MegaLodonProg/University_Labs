; lab3.asm - минимальный каркас
; Задача: прочитать файл и вывести его содержимое на экран
; Пока без переменных окружения, имя файла жестко задано

global _start

section .data
    ; Константы
    SYS_READ    equ 0
    SYS_WRITE   equ 1
    SYS_OPEN    equ 2
    SYS_CLOSE   equ 3
    SYS_BRK     equ 12
    SYS_EXIT    equ 60
    
    O_RDONLY    equ 0
    STDOUT      equ 1
    
    ; Сообщения об ошибках
    err_open    db "Error: cannot open file", 10
    err_open_len equ $ - err_open
    
    err_read    db "Error: cannot read file", 10
    err_read_len equ $ - err_read
    
    ; Имя файла (пока жестко, потом заменим на переменную окружения)
    filename    db "input.txt", 0

section .bss
    ; Буфер для чтения (небольшой, потом будем использовать brk)
    buffer      resb 4096

section .text
_start:
    ; ========== 1. Открыть файл ==========
    mov rax, SYS_OPEN
    mov rdi, filename       ; имя файла
    mov rsi, O_RDONLY       ; только чтение
    mov rdx, 0              ; права не нужны
    syscall
    
    ; Проверка на ошибку
    cmp rax, 0
    jl error_open           ; если отрицательное - ошибка
    
    mov r12, rax            ; сохраняем file descriptor в r12
    
    ; ========== 2. Читать и выводить ==========
read_loop:
    mov rax, SYS_READ
    mov rdi, r12            ; file descriptor
    mov rsi, buffer         ; куда читать
    mov rdx, 4096           ; сколько читать
    syscall
    
    ; Проверка на ошибку
    cmp rax, 0
    jl error_read           ; если отрицательное - ошибка
    
    ; Если прочитали 0 байт - конец файла
    cmp rax, 0
    je close_file
    
    ; Выводим прочитанное
    mov rdx, rax            ; количество прочитанных байт
    mov rax, SYS_WRITE
    mov rdi, STDOUT         ; stdout
    mov rsi, buffer         ; что выводить
    syscall
    
    ; Проверка на ошибку записи
    cmp rax, 0
    jl error_read
    
    jmp read_loop           ; читаем дальше
    
    ; ========== 3. Закрыть файл ==========
close_file:
    mov rax, SYS_CLOSE
    mov rdi, r12
    syscall
    
    ; ========== 4. Выход ==========
    mov rax, SYS_EXIT
    xor rdi, rdi            ; код возврата 0
    syscall

; ========== Обработчики ошибок ==========
error_open:
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, err_open
    mov rdx, err_open_len
    syscall
    
    mov rax, SYS_EXIT
    mov rdi, 1              ; код ошибки 1
    syscall

error_read:
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, err_read
    mov rdx, err_read_len
    syscall
    
    mov rax, SYS_EXIT
    mov rdi, 2              ; код ошибки 2
    syscall