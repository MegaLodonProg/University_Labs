#include "queue.h"
#include "help_func.h"
#include "io.h"

int main()
{
    Queue **queue = NULL; // Queue **arr - (массив с УКАЗАТЕЛЯМИ на структуры)
    Person *arr = NULL; // массив с пассажирами
    int count_person = 0; // общее кол-во пас
    int num_check_in = person_input(&arr, &count_person); // заполняем массив пас + фиксируем кол-во очередей
    if (num_check_in == ERR_EOF || num_check_in == ERR_MEM){
        free_arr(&arr);
        return ERR_EOF;
    }

    if (init(&queue, num_check_in) == ERR_MEM) {
        free_arr(&arr);
        free_queue(&queue, num_check_in);
        return ERR_MEM;
    }

    time_sort_persons(arr, count_person); // сортируем пассажиров по времени прибывания
    int status = create_queue(&queue, arr, count_person, num_check_in);
    if (status == ERR_EOF || status == ERR_MEM) {
        free_arr(&arr);
        free_queue(&queue, num_check_in);
        return 1;
    }
    
    free_queue(&queue, num_check_in);
    free_arr(&arr);

    return 0;
}
