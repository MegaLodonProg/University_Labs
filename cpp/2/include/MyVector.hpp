#pragma once

#include <algorithm>

/**
 * @file MyVector.hpp
 * @brief Шаблонный класс динамического вектора
 * @author Pronin Denis
 * @date 28.09.2025
 */

/**
 * @class MyVector
 * @brief Шаблонный класс динамического массива с автоматическим управлением памятью
 *
 * @tparam T Тип элементов вектора
 *
 * Класс предоставляет функциональность динамического массива с:
 * - Автоматическим расширением при необходимости
 * - Поддержкой семантики перемещения
 * - Проверкой границ доступа
 * - Стандартными операциями для контейнера
 *
 * @details Емкость вектора увеличивается в 2 раза при необходимости добавления новых элементов.
 *          Поддерживает конструкторы копирования и перемещения, операторы присваивания.
 */
template<typename T>
class MyVector {
    T* data = nullptr;
    int size = 0;
    int capacity = 0;

    void extension(int new_capacity);

public:
    /**
     * @brief Конструктор по умолчанию
     */
    MyVector() = default;

    /**
     * @brief Конструктор копирования
     * @param other Другой вектор для копирования
     */
    MyVector(const MyVector& other);

    /**
     * @brief Конструктор перемещения
     * @param other Другой вектор для перемещения
     */
    MyVector(MyVector&& other) noexcept; // если внутри функции вернули и был временный объект

    /**
     * @brief Конструктор из std::vector
     * @param other std::vector для копирования
     */
    MyVector(const std::vector<T>& other);

    /**
     * @brief Деструктор
     */
    ~MyVector(){ delete[] data; }

    /**
     * @brief Оператор присваивания копированием
     * @param other Другой вектор
     * @return Ссылка на текущий объект
     */
    MyVector& operator=(const MyVector& other) noexcept; // MyClass obj1(5), obj2(10); -> obj1 = obj2;

    /**
     * @brief Оператор присваивания перемещением
     * @param other Другой вектор
     * @return Ссылка на текущий объект
     */
    MyVector& operator=(MyVector&& other) noexcept; // MyClass obj1(5); -> obj1 = MyClass(10);

    /**
     * @brief Оператор доступа по индексу (неконстантный)
     * @param index Индекс элемента
     * @return Ссылка на элемент
     */
    T& operator[](int index);

    /**
     * @brief Оператор доступа по индексу (константный)
     * @param index Индекс элемента
     * @return Константная ссылка на элемент
     */
    const T& operator[](int index) const;

    /**
     * @brief Получить емкость вектора
     * @return Емкость вектора
     */
    int get_capacity() const { return capacity; }


    /**
     * @brief Доступ к последнему элементу (неконстантный)
     * @return Ссылка на последний элемент
     * @throws std::out_of_range Если вектор пуст
     */
    T& back();

    /**
     * @brief Доступ к последнему элементу (константный)
     * @return Константная ссылка на последний элемент
     * @throws std::out_of_range Если вектор пуст
     */
    const T& back() const;

    /**
     * @brief Добавление элемента в конец вектора
     * @param student Элемент для добавления
     */
    void push_back(const T& student);

    /**
     * @brief Удаление и возврат последнего элемента
     * @return Удаленный элемент
     * @throws std::out_of_range Если вектор пуст
     */
    T pop_back();

    /**
     * @brief Получить размер вектора
     * @return Размер вектора
     */
    int vec_size() const;

    /**
     * @brief Доступ к элементу с проверкой границ (неконстантный)
     * @param pos Позиция элемента
     * @return Ссылка на элемент
     * @throws std::out_of_range Если позиция вне границ
     */
    T& at(int pos);

    /**
     * @brief Доступ к элементу с проверкой границ (константный)
     * @param pos Позиция элемента
     * @return Константная ссылка на элемент
     * @throws std::out_of_range Если позиция вне границ
     */
    const T& at(int pos) const;

    /**
     * @brief Проверка пустоты вектора
     * @return true если вектор пуст, false иначе
     */
    bool empty() const;
};

// Включаем реализацию
#include "MyVector.tpp"