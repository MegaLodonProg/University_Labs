global main
extern printf, scanf, fopen, fclose, fprintf, exit
extern stderr
extern fabsf

section .data
    fmt_prompt  db "Введите x и eps: ", 0
    fmt_input   db "%f %f", 0
    err_args    db "Ошибка: ожидался 1 аргумент (имя выходного файла).", 10, 0
    err_read    db "Ошибка: не удалось прочитать x и eps.", 10, 0
    err_range   db "Ошибка: |x| должен быть < 1, а eps должен быть > 0.", 10, 0
    err_file    db "Ошибка: не удалось открыть выходный файл.", 10, 0
    file_mode   db "w", 0
    ONE_F       dd 1.0
    fmt_file    db "n=%d term=%.6f", 10, 0
    fmt_out     db "Левая часть: %.6f", 10, "Сумма ряда: %.6f", 10, "Членов: %d", 10, 0

section .bss
    x           resd 1
    eps         resd 1
    file_ptr    resq 1
    saved_argv1 resq 1 ; указатель на строку с именем файла из argv[1]
    x2          resd 1
    term        resd 1 
    sum         resd 1
    n           resd 1
    left_val    resd 1

section .text 
main:
    push rbp ; Сохраняет старое значение базового указателя стека (выравнивание стека)
    mov rbp, rsp

    ; Проверка, что аргументы были переданы (argv[0] — имя программы или полный путь к ней)
    cmp rdi, 2 
    jge read_input
    mov rdi, [rel stderr] ; передаём указатель на стандартный поток ошибок stderr как поток вывода
    lea rsi, [err_args]
    xor eax, eax
    call fprintf
    mov rdi, 1
    call exit

read_input:
    ; Сохранение имени файла из argv[1]
    mov rax, [rsi + 8]
    mov [saved_argv1], rax 

    lea rdi, [fmt_prompt]
    xor eax, eax
    call printf 

    ; Считываем x и eps
    lea rdi, [fmt_input] ; передаём строку формата для scanf
    lea rsi, [x] 
    lea rdx, [eps] 
    xor eax, eax
    call scanf
    cmp eax, 2
    je validate

    mov rdi, [rel stderr]
    lea rsi, [err_read]
    xor eax, eax
    call fprintf
    mov rdi, 2
    call exit

validate:
    ; Загрузка значений в SSE-регистры (регистры для работы с числами с плавающей точкой)
    movss xmm0, [x] 
    movss xmm1, [eps] ; movss - перемещает число с плавающей точкой одинарной точности из памяти в регистр

    ; Проверяем: eps > 0
    xorps xmm2, xmm2
    ucomiss xmm1, xmm2
    ja check_abs_x 

    mov rdi, [rel stderr] 
    lea rsi, [err_range]
    xor eax, eax 
    call fprintf
    mov rdi, 3
    call exit

check_abs_x:
    movss xmm0, [x]
    call fabsf ; берем модуль

    ; Загружаем 1.0 в xmm1 для сравнения
    movss xmm1, [ONE_F]
    ucomiss xmm0, xmm1 
    jb init_loop

    mov rdi, [rel stderr] 
    lea rsi, [err_range] 
    xor eax, eax 
    call fprintf 
    mov rdi, 4
    call exit

init_loop:
    ; Вычисление x^2
    movss xmm0, [x]
    mulss xmm0, [x]
    movss [x2], xmm0

    ; Инициализация переменных ряда
    movss xmm0, [ONE_F] ; term = 1.0 (первый член ряда)
    movss [term], xmm0
    xorps xmm0, xmm0 
    movss [sum], xmm0
    mov dword [n], 0

    ; Открытие файла для записи членов ряда
    mov rdi, [saved_argv1] ; передаёт сохранённый указатель на имя файла в fopen
    lea rsi, [file_mode]
    xor eax, eax
    call fopen
    cmp rax, 0
    jnz file_opened

    mov rdi, [rel stderr] 
    lea rsi, [err_file] 
    xor eax, eax 
    call fprintf
    mov rdi, 5 
    call exit 

file_opened:
    mov [file_ptr], rax ; сохраняем FILE*

loop_start:
    ; Добавление текущего члена к сумме
    movss xmm0, [sum]
    movss xmm1, [term]
    addss xmm0, xmm1
    movss [sum], xmm0

    ; Записываем номер и значение члена в файл
    mov rdi, [file_ptr]
    lea rsi, [fmt_file]
    mov edx, [n] 
    movss xmm0, [term]
    cvtss2sd xmm0, xmm0 ; конвертируем float -> double для %f в printf
    mov eax, 1 ; (SysV ABI) - передаётся один вещественный аргумент в xmm0.
    call fprintf

    ; Проверка условия остановки
    movss xmm0, [term]
    call fabsf
    movss xmm1, [eps]
    ucomiss xmm0, xmm1
    jb calc_left   

    ; Обновление члена ряда по рекуррентной формуле: term *= ((2n+3)/(2n+2)) * x^2
    mov eax, [n]
    shl eax, 1        
    mov ecx, eax         
    add eax, 3    
    add ecx, 2       
    cvtsi2ss xmm2, eax  
    cvtsi2ss xmm3, ecx    
    divss xmm2, xmm3 ; xmm2 = (2n+3)/(2n+2)
    mulss xmm2, [x2] ; xmm2 *= x^2
    movss xmm0, [term]
    mulss xmm0, xmm2    
    movss [term], xmm0

    inc dword [n]
    jmp loop_start    

calc_left:
    ; Закрытие файл после завершения
    mov rdi, [file_ptr]
    call fclose

    ; Вычисление левой части: (1 - x^2)^(-3/2)
    movss xmm0, [x]
    mulss xmm0, [x] ; xmm0 = x^2
    movss xmm2, [ONE_F]
    subss xmm2, xmm0 ; xmm2 = 1 - x^2
    movaps xmm3, xmm2 ; xmm3 = 1 - x^2
    sqrtss xmm2, xmm2 ; xmm2 = sqrt(1 - x^2)
    mulss xmm2, xmm3 ; xmm2 = (1 - x^2)^(3/2)
    movss xmm0, [ONE_F]
    divss xmm0, xmm2 ; xmm0 = 1 / ((1-x^2)*sqrt(1-x^2))
    movss [left_val], xmm0

print_result:
    movss xmm0, [left_val]
    cvtss2sd xmm0, xmm0
    movss xmm1, [sum]
    cvtss2sd xmm1, xmm1

    lea rdi, [fmt_out]
    mov esi, [n]
    mov eax, 2
    call printf

    mov rdi, 0
    call exit