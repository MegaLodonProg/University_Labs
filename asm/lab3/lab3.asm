section .data
    ; Соответствующие номера системных вызовов
    SYS_READ equ 0
    SYS_WRITE equ 1
    SYS_OPEN equ 2
    SYS_CLOSE equ 3
    SYS_EXIT equ 60
    SYS_BRK equ 12 ; Номер системного вызова brk (выделение памяти)
    
    ; Флаги и файловые дескрипторы
    O_RDONLY equ 0  
    STDOUT equ 1    
    STDERR equ 2 
    
    ; сиволы разделители
    CHAR_NEWLINE equ 10 ; '\n'
    CHAR_SPACE equ 32 ; ' ' 
    CHAR_TAB equ 9 ; '\t' 

    BUFFER_CHUNK equ 4096
    
    err_no_env db "Error: FILENAME not set", CHAR_NEWLINE
    err_no_env_len equ $ - err_no_env

    err_open db "Error: cannot open file", CHAR_NEWLINE
    err_open_len equ $ - err_open
    
    err_read db "Error: cannot read file", CHAR_NEWLINE
    err_read_len equ $ - err_read

    err_brk db "Error: cannot allocate memory", CHAR_NEWLINE   
    err_brk_len equ $ - err_brk
    
    env_key db "FILENAME=", 0
    
    space_char db " "
    newline_char db 10

section .bss
    file_fd resq 1 ; дескриптора файла
    buf_start resq 1 ; Адрес начала динамического буфера
    current_brk resq 1 ; Текущая граница выделенной памяти
    buffer_used resq 1 ; Сколько байт буфера использовано

section .text

global _start

_start:
    ; ========== Инициализация памяти ==========
    mov rax, SYS_BRK
    xor rdi, rdi ; brk(0) - узнать текущую границу
    syscall
    cmp rax, 0                   ;  проверка на то, что граница найдена
    jz .brk_error
    mov [buf_start], rax 
    mov [current_brk], rax
    mov qword [buffer_used], 0
    
    add rax, BUFFER_CHUNK
    mov rdi, rax
    mov rax, SYS_BRK                ; Сдвигаем границу тем самым выделяя новую память
    syscall
    cmp rax, 0
    jz .brk_error
    mov [current_brk], rax  
    
    ; ========== Найти переменную окружения FILENAME ==========
    mov rbx, rsp ; указатель для прохода по argv.
    add rbx, 8

; Ищем конец argv 
.find_null:
    cmp qword [rbx], 0
    je .found_null
    add rbx, 8
    jmp .find_null

; Переключаемся на перебор envp с сохранением нумерации
.found_null:
    add rbx, 8
    mov rsi, rbx 
    xor rcx, rcx
    
.search_loop:
    mov rdx, [rsi + rcx*8]  ; rdx - указатель на текущую строку переменной окружения.
    cmp rdx, 0
    jz .not_found
    
    mov rdi, env_key
    mov r8, rdx ; r8 - указатель на текущую переменную

; Посимвольное сравнение переменной окружения с ключевым словом  
.compare:
    mov al, [rdi]
    cmp al, 0 ; Если дошли до 0-терминирующего символа, то строка полностью совпала
    je .found_var
    cmp al, [r8]
    jne .next_var
    inc rdi
    inc r8
    jmp .compare
    
.found_var:
    mov rdi, [buf_start]

.copy: 
    mov al, [r8]
    cmp al, 0
    je .copy_done
    mov [rdi], al
    inc r8
    inc rdi
    jmp .copy

.copy_done:
    mov byte [rdi], 0
    jmp .file_open ; успешно считали имя переменой, переходим к открытию файла
    
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

    ; ========== Открыте файла ==========
.file_open:
    mov rax, SYS_OPEN
    mov rdi, [buf_start]
    mov rsi, O_RDONLY
    mov rdx, 0
    syscall
    cmp rax, 0
    jl error_open
    mov [file_fd], rax ; установление файлового дескриптора

.process_loop:
    mov r12, [buf_start] ; r12 - начало буфера для строки
    xor r13, r13 ; r13 - длина строки
    
read_char:
    ; проверка на расширение перед чтением
    mov rax, [buffer_used]
    add rax, r13
    cmp rax, BUFFER_CHUNK
    jge .expand_buffer
    
    mov rax, SYS_READ 
    mov rdi, [file_fd]   
    lea rsi, [r12 + r13] ; rsi = КУДА читать (адрес в буфере)
    mov rdx, 1 ; rdx = 1 (читать 1 байт)
    syscall
    cmp rax, 0
    je .eof     
    jl error_read 
    
    ; Проверяем на \n
    cmp byte [r12 + r13], CHAR_NEWLINE
    je .line_ready 
    
    inc r13                         ; Переходим к следующему символу
    inc qword [buffer_used]         ; Увеличиваем счётчик использованных байт
    jmp .read_char

expand_buffer:
    mov rax, SYS_BRK  
    mov rdi, [current_brk]  
    add rdi, BUFFER_CHUNK   
    mov rax, SYS_BRK
    syscall             
    cmp rax, 0
    jz .brk_error  
    mov [current_brk], rax  ; Сохраняем новую границу
    jmp .read_char

eof:
    cmp r13, 0
    je .exit_program
    
line_ready:
    mov qword [buffer_used], 0
    
    xor r14, r14 ; r14 = 0 (позиция в строке)
    xor r15, r15 ; r15 = 0 (флаг: было ли выведено слово)
    
tokenize:
    cmp r14, r13 
    jge .line_done 
    
    movzx rax, byte [r12 + r14] ; Загружаем текущий символ
    
    cmp al, CHAR_SPACE    
    je .skip_space
    cmp al, CHAR_TAB        
    je .skip_space
    cmp al, CHAR_NEWLINE        
    je .line_done
    cmp al, 0            
    je .line_done
    
    mov r8, r14 ; r8 = смещение начала слова
    mov r9, 0 ; r9 = длина слова

; Идём по символам, пока не найдём разделитель. Считаем длину слова.  
.word_scan:
    cmp r14, r13
    jge .word_end
    
    movzx rax, byte [r12 + r14] ; Текущий символ
    cmp al, CHAR_SPACE
    je .word_end
    cmp al, CHAR_TAB
    je .word_end
    cmp al, CHAR_NEWLINE
    je .word_end
    cmp al, 0
    je .word_end
    
    inc r9   
    inc r14
    jmp .word_scan
    
.word_end:
    ; Проверка на палиндром
    mov rdi, r12 ; rdi = база буфера
    add rdi, r8 ; rdi += смещение = указатель на слово
    mov rsi, r9 
    call is_palindrome   
    
    cmp rax, 1 
    jne .skip_word
    
    cmp r15, 1
    jne .no_space  
    
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, space_char
    mov rdx, 1
    syscall

; вывод первого слова (без пробела)
.no_space:
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, r12 
    add rsi, r8     
    mov rdx, r9 
    syscall
    mov r15, 1  
    
.skip_word:
    jmp .tokenize 
    
.skip_space:
    inc r14
    jmp .tokenize
    
line_done:
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, newline_char
    mov rdx, 1
    syscall
    
    jmp .process_loop ; Переходим к следующей строке

exit_program:
    ; Освободить память
    mov rax, SYS_BRK
    mov rdi, [buf_start]
    syscall
    
    mov rax, SYS_CLOSE
    mov rdi, [file_fd]
    syscall
    
    mov rax, SYS_EXIT
    xor rdi, rdi
    syscall

.brk_error:
    mov rax, SYS_WRITE
    mov rdi, STDERR
    mov rsi, err_brk
    mov rdx, err_brk_len
    syscall
    mov rax, SYS_EXIT
    mov rdi, 1
    syscall

; rdi = указатель на слово, rsi = длина
is_palindrome:
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    
    cmp rsi, 0
    je .not_palindrome
    
    mov rcx, 0 ; rcx = 0 (левый индекс)
    mov rdx, rsi 
    dec rdx ; rdx = (правый индекс)                  
    
.pal_loop:
    cmp rcx, rdx
    jge .is_palindrome
    
    movzx rbx, byte [rdi + rcx] ; Символ слева
    movzx rax, byte [rdi + rdx] ; Символ справа
    cmp rbx, rax
    jne .not_palindrome 
    
    inc rcx 
    dec rdx 
    jmp .pal_loop
    
.is_palindrome:
    mov rax, 1
    jmp .pal_done
    
.not_palindrome:
    xor rax, rax
    
.pal_done:
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    ret

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