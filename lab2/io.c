#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

int correct_input(int *num){
    char ch;
    while (1)
    {
        int status = 0;
        if ((status = scanf("%d", num)) == 1 && *num > 0){
            if ((ch = getchar()) == '\n'){
                break;
            }
        }
        else if (status == -1){
            return ERR_EOF;
        }
        printf("Invalid input. Try again.\n");
        while ((ch = getchar()) != '\n'){}
    }
    return 0;
}

int correct_line(Person *person) {
    char line[100];
    while (1) {
        if (fgets(line, sizeof(line), stdin) == NULL) {
            return ERR_EOF;
        }
        line[strcspn(line, "\n")] = '\0';

        char id[MAX_LEN_ID + 1];
        int ta, ts;

        if (sscanf(line, "%[^/]/%d/%d", id, &ta, &ts) == 3) {
            if (strlen(id) > MAX_LEN_ID || ta <= 0 || ts <= 0) {
                printf("\nInvalid string format. Try again.\n");
                continue;
            } else {
                strncpy(person->id, id, MAX_LEN_ID);
                person->ta = ta;
                person->ts = ts;

                return 0;
            }

        } else {
            printf("\nInvalid string format. Try again.\n");
        }
    }
}

int person_input(Person **arr, int *count_person){ // получаем количество пас + формируем список пас
    int num_check_in;
    printf("Enter the number of check-in desks: ");
    int status = correct_input(&num_check_in);
    if (status == ERR_EOF) return ERR_EOF;

    *arr = calloc(sizeof(Person), num_check_in * MAX_Q_LEN * 3);
    if (*arr == NULL) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        return ERR_EOF;
    }

    int capacity = 0;
    printf("Enter passenger information (Example - a/1/20):\n");
    while (capacity < num_check_in * MAX_Q_LEN * 3) {
        Person person;
        if (correct_line(&person) == ERR_EOF) {
            break; // Прекращаем ввод, если достигнут EOF
        }

        (*arr)[capacity] = person;
        capacity++;
    }

    *arr = realloc(*arr, sizeof(Person) * capacity); // урезка памяти до необходимого размера
    if (*arr == NULL){
        return ERR_MEM;
    }
    *count_person = capacity; // фиксирование количества человек

    return num_check_in;
}

int create_queue(Queue ***queue, Person *arr, const int count_person, const int num_check_in) {
    int current_time = 0;
    int cur_person = 0, del_person = 0;
    int q_switch = 0, queues_changed = 0;

    while (del_person < count_person) {
        queues_changed = 0;
        // Удаляем пас, которые уходят
        for (int i = 0; i < num_check_in; i++) {
            if (!is_empty((*queue)[i])) {
                Person *current_person = get_person(*queue, i); // вытаскиваем пас из очереди
                if (current_time == current_person->ts) { // если сейчас время его ухода, то он уходит
                    dequeue((*queue)[i], current_person);
                    del_person++;
                    queues_changed = 1;
                }
            }
        }

        // Добавляем пас
        while (cur_person < count_person && arr[cur_person].ta == current_time) { // может прийти несколько в одно и то же время
            if (!is_empty((*queue)[q_switch])) {
                time_leave(*queue, q_switch, (arr + cur_person), 0); // для каждого пас обновляем правильно время ухода
            }
            else {
                time_leave(*queue, q_switch, (arr + cur_person), 1);
            }
            if (enqueue((*queue)[q_switch], arr[cur_person]) == ERR_MEM) return ERR_MEM;

            q_switch = (q_switch + 1) % num_check_in; // Переключаемся на следующую очередь
            cur_person++;
            queues_changed = 1;
        }

        if (queues_changed) print_queue(*queue, num_check_in, current_time);

        current_time++;
    }

    return 0;
}
