#include <iostream>
#include "Master.hpp"

Master::Master(const std::string &name, unsigned int age, double multiplier)
    : Worker(name, age), multiplier_(multiplier) {

    if (!is_correct_multiplier(multiplier)) {
        throw std::invalid_argument("Multiplier must be between 1.0 and 5.0");
    }
};


bool Master::is_correct_multiplier(double multiplier) {
    return (1.0 < multiplier) && (multiplier <= 5.0);
}

void Master::set_multiplier(double new_multiplier) {
    if (!is_correct_multiplier(new_multiplier)) {
        throw std::invalid_argument("Multiplier must be between 1.0 and 5.0");
    }
    multiplier_ = new_multiplier;
}

void Master::to_json(json& j) const {
    Worker::to_json(j);
    j["role"] = "Мастер";
    j["multiplier"] = multiplier_;
}