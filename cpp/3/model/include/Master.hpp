#pragma once

#include "Worker.hpp"

/**
 * @class Master
 * @brief Класс, представляющий мастера в бригаде
 *
 * Наследуется от базового класса Worker и добавляет характеристику мультипликативного вклада
 * в увеличение дневной выработки рабочих.
 *
 * @details Мастер характеризуется значением множителя в диапазоне (1.0..5.0], который
 *          применяется как мультипликативный вклад в расчете выработки на участке работ.
 */
class Master : public Worker {
    double multiplier_;

public:
    /**
     * @brief Конструктор мастера
     * @param name Имя мастера
     * @param age Возраст мастера
     * @param multiplier Мультипликатор выработки в диапазоне (1.0..5.0]
     * @throws std::invalid_argument Если мультипликатор не входит в допустимый диапазон
     */
    Master(const std::string &name, unsigned int age, double multiplier);

    Master(Master&&) = default;
    Master& operator=(Master&&) = default;
    ~Master() override = default;

    /**
     * @brief Получить значение мультипликатора выработки
     * @return Текущее значение мультипликатора
     */
    double get_multiplier() const {return multiplier_;};

    /**
     * @brief Установить новое значение мультипликатора выработки
     * @param new_multiplier Новое значение мультипликатора в диапазоне (1.0..5.0]
     * @throws std::invalid_argument Если новое значение не входит в допустимый диапазон
     */
    void set_multiplier(double new_multiplier);

    /**
     * @brief Получить роль работника
     * @return Строку "Мастер"
     * @note Переопределяет виртуальный метод базового класса Worker
     */
    std::string get_role() const override { return "Мастер"; }

    /**
     * @brief Сериализовать объект мастера в JSON
     * @param j JSON объект для заполнения
     */
    void to_json(json& j) const override;

private:
    static bool is_correct_multiplier(double multiplier) ;
};