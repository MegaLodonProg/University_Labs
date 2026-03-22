#pragma once

#include "Worker.hpp"

/**
 * @class Workman
 * @brief Класс, представляющий обычного рабочего в бригаде
 *
 * Наследуется от базового класса Worker и добавляет характеристику аддитивного вклада
 * в увеличение дневной выработки группы на объекте.
 *
 * @details Рабочий характеризуется значением продуктивности в диапазоне [1..10], которое
 *          представляет собой аддитивный вклад в общую выработку на участке работ.
 */
class Workman : public Worker {
    unsigned int productivity_;

public:
    /**
     * @brief Конструктор рабочего
     * @param name Имя рабочего
     * @param age Возраст рабочего
     * @param productivity Продуктивность рабочего в диапазоне [1..10]
     * @throws std::invalid_argument Если продуктивность не входит в допустимый диапазон
     */
    Workman(const std::string &name, unsigned int age, unsigned int productivity);

    // есть доп поля (productivity_), поэтому нужно явно определить
    Workman(Workman&&) = default;
    Workman& operator=(Workman&&) = default;
    ~Workman() override = default;

    /**
     * @brief Получить значение продуктивности рабочего
     * @return Текущее значение продуктивности в диапазоне [1..10]
     */
    unsigned int get_productivity() const {return productivity_;};

    /**
     * @brief Установить новое значение продуктивности рабочего
     * @param new_productivity Новое значение продуктивности в диапазоне [1..10]
     * @throws std::invalid_argument Если новое значение не входит в допустимый диапазон
     */
    void set_productivity(unsigned int new_productivity);

    /**
     * @brief Сериализовать работника в JSON объект
     * @param j JSON объект для заполнения данными работника
     */
    void to_json(json& j) const override;


    std::string get_role() const override { return "Рабочий"; }

private:
    static bool is_correct_productivity(unsigned int productivity);
};