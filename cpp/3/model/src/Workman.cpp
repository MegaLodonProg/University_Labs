#include <iostream>
#include "Workman.hpp"

Workman::Workman(const std::string &name, unsigned int age, unsigned int productivity)
    : Worker(name, age), productivity_(productivity) {

    if (!is_correct_productivity(productivity)) {
        // при исключении автоматически вызывается деструктор Worker
        throw std::invalid_argument("Productivity must be between 1 and 10");
    }
};


bool Workman::is_correct_productivity(unsigned int productivity) {
    return (1 <= productivity) && (productivity <= 10);
}

void Workman::set_productivity(unsigned int new_productivity) {
    if (!is_correct_productivity(new_productivity)) {
        throw std::invalid_argument("Productivity must be between 1 and 10");
    }
    productivity_ = new_productivity;
}

void Workman::to_json(json& j) const {
    Worker::to_json(j);
    j["role"] = "Рабочий";
    j["productivity"] = productivity_;
}