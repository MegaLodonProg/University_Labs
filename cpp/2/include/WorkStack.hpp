#pragma once

#include "StudentWork.hpp"
#include "MyVector.hpp"

/**
 * @file WorkStack.hpp
 * @brief Стек для управления студенческими работами
 * @author Pronin Denis
 * @date 28.09.2025
 */

/**
 * @class WorkStack
 * @brief Стек для хранения и управления студенческими работами
 *
 * Класс реализует функциональность стопки студенческих работ с возможностью:
 * - Добавления и извлечения работ
 * - Объединения разбитых работ одного студента
 * - Разбиения работ на отдельные страницы
 * - Поиска работ без оценок
 *
 * @details Использует MyVector<StudentWork> для хранения работ.
 *          Поддерживает операции ввода/вывода через перегруженные операторы.
 */
class WorkStack {
    MyVector<StudentWork> stack;

    void sort_works(MyVector<StudentWork>& works);

public:
    /**
     * @brief Конструктор по умолчанию
     */
    WorkStack() = default;

    /**
     * @brief Конструктор с инициализацией из массива работ
     * @param works Массив работ
     * @param count Количество работ в массиве
     */
    WorkStack(const StudentWork* works, size_t count);

    /**
     * @brief Конструктор с инициализацией из вектора работ
     * @param works Вектор работ
     */
    WorkStack(const MyVector<StudentWork>& works);

    /**
     * @brief Извлечение работы с вершины стопки
     * @return Работа с вершины стопки
     * @throws std::out_of_range Если стопка пуста
     */
    StudentWork pop();

    /**
     * @brief Извлечение следующей работы без оценки
     * @return Работа без оценки
     * @throws std::runtime_error Если нет работ без оценки
     */
    StudentWork pop_without_grade();

    /**
     * @brief Проверка пустоты стопки
     * @return true если стопка пуста, false иначе
     */
    bool empty() const;

    /**
     * @brief Проверка частичной заполненности стопки
     * @return true если стопка не пуста, false иначе
     */
    bool partially_filled() const;

    /**
     * @brief Объединение всех разбитых на части работ
     * @details Для каждого студента объединяет все его части работ в одну
     */
    void merge_fragmented_works();

    /**
     * @brief Разбиение всех работ в стопке на отдельные листы
     * @return Новая стопка с работами по одному листу каждая
     */
    WorkStack split_all_pages() const;

    /**
     * @brief Добавление новой работы на вершину стопки
     * @param work Работа для добавления
     * @return Ссылка на текущий объект
     */
    WorkStack& operator+=(const StudentWork& work);

    /**
     * @brief Оператор присваивания копированием
     * @param other Другой объект WorkStack
     * @return Ссылка на текущий объект
     */
    WorkStack& operator=(const WorkStack& other) noexcept;

    friend std::ostream& operator<<(std::ostream& os, const WorkStack& works);
    friend std::istream& operator>>(std::istream& is, WorkStack& works);
};