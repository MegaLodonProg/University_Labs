; lab3.asm - исправленная версия
global _start

section .data
    SYS_READ    equ 0
    SYS_WRITE   equ 1
    SYS_OPEN    equ 2
    SYS_CLOSE   equ 3
    SYS_EXIT    equ 60
    
    O_RDONLY    equ 0           ; флаг только для чтения
    STDOUT      equ 1           ; файловый дескриптор стандартного вывода
    STDERR      equ 2           ; файловый дескриптор стандартного вывода ошибок
    
    err_no_env  db "Error: FILENAME not set", 10
    err_no_env_len equ $ - err_no_env
    
    err_open    db "Error: cannot open file", 10
    err_open_len equ $ - err_open
    
    err_read    db "Error: cannot read file", 10
    err_read_len equ $ - err_read
    
    ; Ключ включает '=', поэтому после совпадения указатель будет на значении
    env_key     db "FILENAME=", 0

section .bss
    filename    resb 256
    buffer      resb 4096

section .text
_start:
    ; ========== 1. Найти переменные окружения ==========
    mov rbx, rsp                ; rbx указывает на argc
    add rbx, 8                  ; rbx указывает на argv[0]


; Стек при старте программы выглядит так:
; [rsp]      = argc
; [rsp+8]    = argv[0]
; [rsp+16]   = argv[1]
; ...
; [rsp+...]  = NULL (конец argv)
; [rsp+...]  = envp[0] (начало переменных окружения)


; пропускаем argc до начала envp
.find_null:
    cmp qword [rbx], 0          ; проверяем, не NULL ли (конец argv)
    je .found_null
    add rbx, 8                  ; следующий указатель
    jmp .find_null
    
.found_null:
    add rbx, 8                  ; теперь rbx указывает на envp[0]
    
    ; Теперь ищем FILENAME= среди envp
    mov rsi, rbx                ; rsi = начало envp
    xor rcx, rcx                ; счетчик
    
.search_loop:
    mov rdx, [rsi + rcx*8]      ; берем очередную строку envp
    test rdx, rdx               ; проверяем на NULL (конец envp)
    jz .not_found
    
    ; Сравниваем с "FILENAME="
    mov rdi, env_key            ; что ищем (указатель на ключ)
    mov r8, rdx                 ; текущая строка из envp (указатель)
    
.compare:
    mov al, [rdi]               ; символ из ключа
    cmp al, 0                   ; дошли до конца ключа?
    je .found_var               ; ДА: ключ полностью совпал, переходим к копированию
    
    cmp al, [r8]                ; сравниваем с символом из строки окружения
    jne .next_var               ; НЕТ: не совпало, следующая переменная
    
    inc rdi
    inc r8
    jmp .compare
    
.found_var:
    ; Если мы здесь, значит ключ "FILENAME=" полностью совпал.
    ; Регистр r8 сейчас указывает на первый символ значения (после '=')
    mov rdi, filename           ; куда копируем
    
.copy:
    mov al, [r8]
    cmp al, 0
    je .copy_done
    mov [rdi], al
    inc r8
    inc rdi
    jmp .copy
    
.copy_done:
    mov byte [rdi], 0           ; нуль-терминатор для имени файла
    jmp .file_open
    
.next_var:
    inc rcx
    jmp .search_loop
    
.not_found:
    mov rax, SYS_WRITE
    mov rdi, STDERR
    mov rsi, err_no_env
    mov rdx, err_no_env_len
    syscall
    mov rax, SYS_EXIT
    mov rdi, 1
    syscall

    ; ========== 2. Открыть файл ==========
.file_open:
    mov rax, SYS_OPEN
    mov rdi, filename
    mov rsi, O_RDONLY
    mov rdx, 0
    syscall
    
    cmp rax, 0
    jl error_open
    
    mov r12, rax                ; сохраняем file descriptor
    
    ; ========== 3. Читать и выводить ==========
.read_loop:
    mov rax, SYS_READ
    mov rdi, r12
    mov rsi, buffer
    mov rdx, 4096
    syscall
    
    cmp rax, 0
    jl error_read
    
    cmp rax, 0
    je .close
    
    mov rdx, rax
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, buffer
    syscall
    
    jmp .read_loop
    
.close:
    mov rax, SYS_CLOSE
    mov rdi, r12
    syscall
    
    mov rax, SYS_EXIT
    xor rdi, rdi
    syscall

; ========== Обработчики ошибок ==========
error_open:
    mov rax, SYS_WRITE
    mov rdi, STDERR
    mov rsi, err_open
    mov rdx, err_open_len
    syscall
    mov rax, SYS_EXIT
    mov rdi, 1
    syscall

error_read:
    mov rax, SYS_WRITE
    mov rdi, STDERR
    mov rsi, err_read
    mov rdx, err_read_len
    syscall
    mov rax, SYS_EXIT
    mov rdi, 1
    syscall