#pragma once

#include "Worker.hpp"

/**
 * @class Foreman
 * @brief Класс, представляющий бригадира в бригаде
 *
 * Наследуется от базового класса Worker. Бригадир не участвует в работах напрямую
 * и не имеет дополнительных характеристик продуктивности или множителя.
 *
 * @details В бригаде всегда есть один и только один бригадир. Класс не добавляет
 *          новых полей к базовому классу Worker.
 */
class Foreman : public Worker {

public:
    /**
     * @brief Конструктор бригадира
     * @param name Имя бригадира
     * @param age Возраст бригадира
     */
    Foreman(const std::string &name, unsigned int age) : Worker(name, age) {};

    // нет доп полей, остальные конструкторы наследуются от Worker
    ~Foreman() override = default;

    /**
     * @brief Получить роль работника
     * @return Строку "Бригадир"
     * @note Переопределяет виртуальный метод базового класса Worker
     */
    std::string get_role() const override { return "Бригадир"; }


    /**
     * @brief Сериализовать объект бригадира в JSON
     * @param j JSON объект для заполнения
     */
    void to_json(json& j) const override {
        Worker::to_json(j);
        j["role"] = "Бригадир";
    }

};