section .data

rows: db 10 ; беззнаковые числа
cols: db 10 

ROWS_COUNT equ 10
COLS_COUNT equ 10

align 8 ; отступ для оптимизации, адрес данных должен быть кратен их размеру (8 байт) - тогда считывание за 1 раз происходит

matrix: dq 1, 2, 3, 4, 5, 6, 7, 8, 9, 0
        dq 0, 9, 8, 7, 6, 5, 4, 3, 2, 1
        dq 2, -4, 6, 8, 0, 9, 7, 15, 3, 1
        dq 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
        dq 1, 3, 5, 7, -9, 8, 6, 4, 2, 0
        dq 0, 2, 4, 6, 8, 9, 7, 5, 3, 1
        dq 5, 5, 5, 5, 35, 5, 5, -5, 5, 5
        dq 1, 9, 2, 8, 3, 7, 4, 6, 5, 0
        dq 8, 7, 26, 5, 4, 3, 52, 1, 0, 19
        dq 3, -6, 9, 2, 5, 8, 1, 4, 7, 0

temp_min: dq 0 ; место для нахождения минимума в каждом столбце
shift_next: dq 0 ; место для расчета сдвига

; элемент [0][1] - matrix + 8, элемент [1][0] matrix + (cols * 8)ы

section .bss

align 8

; директива resq обрабатываются на этапе сборки, [cols] - это значение станет известно только во время выполнения программы
min_vals: resq COLS_COUNT ; массив с минимальными элементами
ptr_arr: resq COLS_COUNT ; массив с адресами начал стобцов
col_buffer: resq ROWS_COUNT ; буфер для перестановки столбцов в матрице

section .text

global _start

_start: 
    ; ---------- Необходимые значения ----------
    xor rcx, rcx
    mov rax, 8
    mov cl, [cols]
    mul rcx ; сдвиг = (8 * cols) байт
    mov [shift_next], rax ; сдвиг для следующего элемента в столбце

    ; ---------- Начальная инициализация ----------
    xor rcx, rcx
    mov cl, [cols]        ; кладем количество стобцов в переменную-счетчик для цикла
    mov rax, matrix       ; rax = адрес первого элемента матрицы 
    mov rbx, ptr_arr      ; rbx = адрес начала массива указателей (кладем адрес!!!)
    mov rbp, min_vals     ; rbp = адрес начала массива с минимальными эл. в столбцах

    ; call - позволяет запомнить место, куда надо вернутся после завершения подмодуля в отличии от jmp
    call fill_ptrs 
    call fill_min_vals
    call sort_col
    call modify_matrix

    ; ---------- Выход ----------
exit:
    mov edi, 0
    mov eax, 60
    syscall


; Заполняет ptr_arr адресами столбцов
fill_ptrs:
    push rax
    push rbx
    push rcx ; сохраняем значение rcx для следующих циклов 

.loops:
    mov [rbx], rax ; положить 8 байт из RAX по тому адресу, который лежит в RBX. Положит адрес столбца
    add rbx, 8   ; получаем следующую ячеку в нашем буфере для следующего столбца
    add rax, 8   ; переходим к следующему элементу в матрице, который задает начало нового столбца
    loop .loops  ; уменьшаем rcx и повторяем, пока rcx != 0

    pop rcx ; восстанавливаем значение счетчика
    pop rbx ; восстанавливаем указатели на начало массивов
    pop rax
    ret ; возвращаемся к общему потоку функций

; .loop — это локальная метка. В NASM метки, начинающиеся с точки, считаются локальными и привязаны к последней глобальной метке.


; Заполняет массив минимальных элемнтов в столбцах
fill_min_vals:
    ; push rax
    ; push rbx
    ; push rbp
    ; push rcx 

    mov cl, [rows] ; теперь длину столбца храним как счетчик - итерируемся в пределах столбца
    dec cl ; rows - 1 сравнений
    mov rdx, [rax]        ; прочитать значение из памяти по адресу rax (певый элемент столбца) в rdx
    mov [temp_min], rdx   ; временный минимум в столбце

    xor r8, r8
    movzx r9, byte [cols] ; счетчик для внешнего цикла

.loop1:
    cmp r8, r9 ; провекра внешнего счетчика (обошли ли все столбцы?)
    jge .done
    push rax
    push rcx
    mov rax, [ptr_arr + r8*8] ; двигаемся по массиву с адресами столбцов
    mov rdx, [rax]        ; прочитать значение из памяти по адресу rax (певый элемент столбца) в rdx
    mov [temp_min], rdx   ; записать это значение в temp_min
    jmp .loop2

.loop2:    
    add rax, [shift_next] ; переходим к следующему эл в столбце для сравнения
    mov rdx, [rax] ; rdx = значение следующего эл
    cmp rdx, [temp_min] ; сравнить temp_min с rdx
    jge .next    ; если rdx >= temp_min, пропускаем замену
    mov [temp_min], rdx

    .next:
    loop .loop2 

    inc r8 ; увеличиваем счетчик для внешнего цикла
    mov rdx, [temp_min]
    mov [rbp], rdx ; положили минимум в массив с мин эл
    add rbp, 8

    pop rcx
    inc rcx ; тк loop .loop1 тоже уменьшает на 1 rcx
    pop rax
    loop .loop1

.done:
    ; pop rcx
    ; pop rbp
    ; pop rbx
    ; pop rax
    ret


; Сортировка массива (even-odd) с мин эл синхронно с адресами соответств столбцов 
sort_col:
    movzx r12, byte [cols]     ; r12 = количество столбцов
    mov r13, 1                  ; флаг: были ли перестановки - Начинаем с 1, чтобы зайти в цикл

.main_loop:
    cmp r13, 0 ; проверка на то, что за проход была хоть одна перестановка
    je .done
    xor r13, r13

    ; ---------- Четная фаза ----------
    ; Сравниваем пары (0,1), (2,3), (4,5)...
    mov r14, 0 ; r14 = начальный индекс для четной фазы
    call .phase ; Вызываем проходку фазы
    
    ; ---------- Нечетная фаза ----------
    ; Сравниваем пары (1,2), (3,4), (5,6)...
    mov r14, 1 ; r14 = начальный индекс для нечетной фазы 
    call .phase
    
    jmp .main_loop 

.phase:
    ; r15 = i+1 (второй элемент пары)
    mov r15, r14
    inc r15 ; r15 = r14 + 1

.phase_loop:
    ; Проверяем, не вышли ли за границы массива
    cmp r15, r12
    jge .phase_done ; Если i+1 >= cols, выходим из фазы (все пары этой фазы обработаны)

    ; Загружаем значения для сравнения
    mov rax, [min_vals + r14*8] ; rax = min_vals[i]
    mov rbx, [min_vals + r15*8] ; rbx = min_vals[i+1]

    ; ---------- Выбор направления сортировки ----------
    %ifdef ASC
        ; По возрастанию: min_vals[i] <= min_vals[i+1]
        cmp rax, rbx
        jle .phase_next
    %else
        ; По убыванию: min_vals[i] >= min_vals[i+1]
        cmp rax, rbx
        jge .phase_next
    %endif

    ; Меняем местами min_vals[i] и min_vals[i+1]
    mov rdx, [min_vals + r14*8]
    mov rcx, [min_vals + r15*8]
    mov [min_vals + r14*8], rcx
    mov [min_vals + r15*8], rdx

    ; Меняем местами ptr_arr[i] и ptr_arr[i+1] (СИНХРОННО!)
    mov rdx, [ptr_arr + r14*8]
    mov rcx, [ptr_arr + r15*8]
    mov [ptr_arr + r14*8], rcx
    mov [ptr_arr + r15*8], rdx

    ; Устанавливаем флаг, что были перестановки
    mov r13, 1

.phase_next:
    ; Переходим к следующей паре
    add r14, 2 
    add r15, 2         
    jmp .phase_loop ; Повторяем для следующей пары
    
.phase_done:
    ret

.done:
    ret


; Модификация исходной матрицы
modify_matrix:
    ; Загружаем размеры
    movzx r12, byte [cols]  
    movzx r13, byte [rows]   
    
    ; Буфер для временного хранения столбца при перестановке
    mov r14, col_buffer ; r14 = адрес буфера
    
    xor r15, r15 ; r15 = i (текущий индекс индекс столбца)
    
.main_loop:
    cmp r15, r12  ; Все столбцы обработали?
    jge .done 
    
    ; ---------- Сохраняем текущий i-й столбец матрицы в буфер ----------
    ; Вычисляем адрес i-го столбца в матрице: matrix + i*8
    lea rax, [matrix + r15*8] ; rax = адрес i-го столбца, а не значение по этому адресу благодаря lea
    
    push r15 ; сохраняем i
    push rax ; сохраняем адрес i-го столбца
    
    ; Копируем из матрицы (rax) в буфер (r14)
    mov rsi, rax ; i-й столбец в матрице
    mov rdi, r14 ; в буфер
    mov rcx, r13 ; сколько: rows (длина столбца)
    call copy_column_to_buffer
    
    pop rax  ; восстанавливаем данные
    pop r15  
    
    ; ---------- Копируем столбец из ptr_arr[i] на место i-го столбца ----------
    mov rbx, [ptr_arr + r15*8] ; rbx = адрес столбца, который должен быть на позиции i

    ; Находим индекс j, где ptr_arr[j] == matrix + r15*8 (исходный адрес i-го столбца из самой матрицы)
    lea rdx, [matrix + r15*8] ; Это адрес, который физически соответствует i-му столбцу в матрице.
    
    xor rcx, rcx

; Находим индекс j, где ptr_arr[j] == matrix + r15*8 (исходный адрес i-го столбца из самой матрицы) 
.find_j:
    cmp [ptr_arr + rcx*8], rdx ; ptr_arr[j] == matrix + i*8 ?
    je .found_j
    inc rcx
    jmp .find_j
    
.found_j:
    ; rcx = j (индекс, откуда берем правильный столбец)
    push r15
    push rax
    push rbx
    push rcx
    
    ; Копируем из источника (rbx) в назначение (rax - i-й столбец матрицы)
    mov rsi, rbx                   ; откуда: столбец из ptr_arr[i]
    mov rdi, rax                   ; куда: i-й столбец в матрице
    mov rcx, r13                   ; сколько: rows (длина столбца)
    call copy_column_to_matrix     ; копируем столбец в матрицу (с шагом shift_next)
    
    pop rcx
    pop rbx
    pop rax
    pop r15
    
    ; ---------- Копируем из буфера на место, откуда взяли столбец ----------
    
    push r15
    push rbx
    push rcx
    
    ; Копируем из буфера (r14) в место, откуда взяли столбец (rbx)
    mov rsi, r14                  
    mov rdi, rbx                   
    mov rcx, r13                   
    call copy_column_to_matrix_from_buf    
    
    pop rcx
    pop rbx
    pop r15
    

    ; ---------- Обновляем ptr_arr ----------
    ; Меняем местами указатели в ptr_arr[i] и ptr_arr[j]
    push r15
    push rcx
    
    mov rax, [ptr_arr + r15*8]
    mov rdx, [ptr_arr + rcx*8]
    mov [ptr_arr + r15*8], rdx
    mov [ptr_arr + rcx*8], rax
    
    pop rcx
    pop r15

    ; Переходим к следующему столбцу
    inc r15
    jmp .main_loop
    
.done:
    ret

; копируем столбец из матрицы в буфер
copy_column_to_buffer:
    push rsi
    push rax
    mov rdx, [shift_next]           ; шаг до следующей строки в матрице
    
.buffer_loop:
    mov rax, [rsi]                  ; читаем элемент из матрицы
    mov [rdi], rax                   ; записываем в буфер
    add rsi, rdx                     ; следующая строка в матрице
    add rdi, 8                       ; следующая ячейка в буфере
    loop .buffer_loop
    
    pop rax
    pop rsi
    ret


; копируем один столбец из матрицы на место другого 
copy_column_to_matrix:
    push rax
    mov rdx, [shift_next]           ; шаг до следующей строки в матрице
    
.matrix_loop:
    mov rax, [rsi]                  ; читаем элемент из источника (столбец матрицы)
    mov [rdi], rax                   ; записываем в матрицу
    add rsi, rdx                       ; следующий элемент в источнике (столбец матрицы)
    add rdi, rdx                     ; следующая строка в матрице
    loop .matrix_loop
    
    pop rax
    ret

; копируем столбец из буфера в матрицу
copy_column_to_matrix_from_buf:
    push rax
    
    mov rdx, [shift_next]           ; шаг до следующей строки в матрице
    
.matrix_loop:
    mov rax, [rsi]                  ; читаем элемент из источника (буфера)
    mov [rdi], rax                   ; записываем в матрицу
    add rsi, 8                       ; следующий элемент в источнике (буфер идет подряд)
    add rdi, rdx                     ; следующая строка в матрице
    loop .matrix_loop
    
    pop rax
    ret