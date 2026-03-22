#pragma once

#include <iostream>
#include <optional>
#include <vector>

/**
 * @file StudentWork.hpp
 * @brief Класс для представления студенческой работы
 * @author Pronin Denis
 * @date 28.09.2025
 */

/**
 * @class StudentWork
 * @brief Представляет студенческую работу с фамилией студента, оценкой и диапазоном страниц
 *
 * Класс инкапсулирует данные о студенческой работе:
 * - Фамилия студента
 * - Оценка (может быть не выставлена)
 * - Номера первой и последней страниц работы
 *
 * @details Оценка может присутствовать только на работе, начинающейся с 1-й страницы.
 *          Разные части одной работы (разные диапазоны страниц) могут быть объединены.
 *          Поддерживает проверку корректности данных и операции сравнения.
 */
class StudentWork {
    std::string surname;
    std::optional<int> grade;
    int first_page {};
    int last_page {};

    // Important : Класс сам себя валидирует - принцип ИНКАПСУЛЯЦИИ
    // static - Принадлежность классу, а не объекту: Метод существует даже если не создано ни одного объекта класса. Нет доступа к this
    static bool is_valid_grade(const int g) {
        return (g >= 2 && g <= 5);
    }
    static bool is_valid_pages(const int first, const int last) {
        return (first > 0) && (last >= first);
    }

public:

    /**
     * @brief Конструктор по умолчанию
     */
    StudentWork() = default;

    /**
     * @brief Конструктор с инициализацией только фамилии
     * @param surname Фамилия студента
     */
    StudentWork(std::string surname) : surname(std::move(surname)), grade(std::nullopt), first_page(1), last_page(1){}

    /**
     * @brief Конструктор с полной инициализацией
     * @param surname Фамилия студента
     * @param grade Оценка (2-5) или std::nullopt если не выставлена
     * @param first_page Номер первого листа
     * @param last_page Номер последнего листа
     * @throws std::invalid_argument Если оценка выставлена не на первом листе или неверные номера страниц
     */
    StudentWork(std::string  surname, std::optional<int> grade, int first_page, int last_page);

    // [[nodiscard]] - побуждает не игнорировать результат. Выдает предупреждение.

    /**
     * @brief Получить фамилию студента
     * @return Фамилия студента
     */
    [[nodiscard]] std::string get_surname() const {return surname;}

    /**
     * @brief Получить оценку
     * @return Оценка или std::nullopt если не выставлена
     */
    [[nodiscard]] std::optional<int> get_grade() const {return grade;}

    /**
     * @brief Получить номер первого листа
     * @return Номер первого листа
     */
    [[nodiscard]] int get_first_page() const {return first_page;}

    /**
     * @brief Получить номер последнего листа
     * @return Номер последнего листа
     */
    [[nodiscard]] int get_last_page() const {return last_page;}

    /**
     * @brief Установить фамилию студента
     * @param new_sur Новая фамилия
     */
    void set_surname(const std::string& new_sur);

    /**
     * @brief Установить оценку
     * @param new_g Новая оценка (2-5) или std::nullopt
     * @throws std::invalid_argument Если оценка устанавливается не на первом листе или неверное значение
     */
    void set_grade(std::optional<int> new_g);

    /**
     * @brief Установить номера страниц
     * @param new_first Новый номер первого листа
     * @param new_last Новый номер последнего листа
     * @throws std::invalid_argument Если номера страниц невалидны
     */
    void set_pages(int new_first, int new_last);

    /**
    * @brief Сравнить две работы
    * @param other Другая работа для сравнения
    * @return Отрицательное число если this < other, 0 если равны, положительное если this > other
    */
    int compare(const StudentWork& other) const;

    bool operator==(const StudentWork& other) const {return compare(other) == 0;};
    bool operator!=(const StudentWork& other) const {return compare(other) != 0;};
    bool operator<(const StudentWork& other) const {return compare(other) < 0;};
    bool operator<=(const StudentWork& other) const {return compare(other) <= 0;};
    bool operator>(const StudentWork& other) const {return compare(other) > 0;};
    bool operator>=(const StudentWork& other) const {return compare(other) >= 0;};

    /**
     * @brief Ввод данных работы из потока
     * @param is Входной поток
     * @throws std::runtime_error Если ошибка парсинга данных
     */
    void input(std::istream& is = std::cin);

    /**
     * @brief Вывод данных работы в поток
     * @param os Выходной поток
     */
    void output(std::ostream& os = std::cout) const;

    // Это глобальные функции, а не методы StudentWork! Вызвается не work.operator<<()
    // Объявление friend необходимо, чтобы операторы имели доступ к приватным полям класса - но можно и геттеры юзать.
    friend std::ostream& operator<<(std::ostream& os, const StudentWork& work);
    friend std::istream& operator>>(std::istream& is, StudentWork& work);

    /**
     * @brief Объединение двух частей работы в одну
     * @param other Другая работа для объединения
     * @return Новая объединенная работа
     * @throws std::logic_error Если работы нельзя объединить (разные студенты или не соседние части)
     */
    StudentWork operator+(const StudentWork& other) const;

    /**
     * @brief Разбиение работы на отдельные листы
     * @return Вектор работ, каждая из одного листа
     */
    std::vector<StudentWork> split_into_pages() const;

};
