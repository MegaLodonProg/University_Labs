#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "Foreman.hpp"
#include "Master.hpp"
#include "Workman.hpp"
#include "Worker.hpp"

using namespace boost::uuids;

Worker::Worker(const std::string& name, unsigned int age)
    : name_(name), age_(age) {
    // Создает генератор случайных UUID
    static random_generator generator;
    id_ = generator();
}

std::string Worker::get_id_string() const {
    return to_string(id_);
};

bool Worker::is_friend(const uuid& other_id) const {
    return std::find(friends_.begin(), friends_.end(), other_id) != friends_.end();
}

bool Worker::is_enemy(const uuid& other_id) const {
    return std::find(enemies_.begin(), enemies_.end(), other_id) != enemies_.end();
}

void Worker::add_friend(const uuid& friend_id) {
    if (friend_id == id_) {
        throw std::invalid_argument("Cannot add yourself as friend");
    };

    if (is_friend(friend_id)) {
        throw std::invalid_argument("Already friends with this worker");
    }

    if (is_enemy(friend_id)) {
        throw std::invalid_argument("Already enemies with this worker");
    }

    friends_.push_back(friend_id);
}

void Worker::remove_friend(const uuid& friend_id) {
    const auto it = std::find(friends_.begin(), friends_.end(), friend_id);
    if (it == friends_.end()) {
        throw std::invalid_argument("Friend not found");
    }

    friends_.erase(it);
}

void Worker::remove_enemy(const uuid& enemy_id) {
    const auto it = std::find(enemies_.begin(), enemies_.end(), enemy_id);
    if (it == enemies_.end()) {
        throw std::invalid_argument("Enemy not found");
    }

    enemies_.erase(it);
}

void Worker::add_enemy(const uuid& enemy_id) {
    if (enemy_id == id_) {
        throw std::invalid_argument("Cannot add yourself as enemy");
    };

    if (is_enemy(enemy_id)) {
        throw std::invalid_argument("Already enemies with this worker");
    }

    if (is_friend(enemy_id)) {
        throw std::invalid_argument("Already friends with this worker");
    }

    enemies_.push_back(enemy_id);
}

void Worker::to_json(json& j) const {
    j["id"] = get_id_string();
    j["name"] = name_;
    j["age"] = age_;
    j["friends"] = std::vector<std::string>();
    for (const auto& id : friends_) {
        j["friends"].push_back(to_string(id));
    }
    j["enemies"] = std::vector<std::string>();
    for (const auto& id : enemies_) {
        j["enemies"].push_back(to_string(id));
    }
}

void Worker::from_json(const json& j) {
    name_ = j.at("name").get<std::string>();
    age_ = j.at("age").get<unsigned int>();
    friends_.clear();
    for (const auto& s : j.at("friends")) {
        friends_.push_back(string_generator()(s.get<std::string>()));
    }
    enemies_.clear();
    for (const auto& s : j.at("enemies")) {
        enemies_.push_back(string_generator()(s.get<std::string>()));
    }
}

std::unique_ptr<Worker> Worker::from_json_static(const json& j) {
    std::string role = j.at("role").get<std::string>();
    if (role == "Рабочий") {
        auto w = std::make_unique<Workman>("", 0, 1);
        w->from_json(j);
        w->set_productivity(j.at("productivity").get<unsigned int>());
        return w;
    } else if (role == "Мастер") {
        auto w = std::make_unique<Master>("", 0, 1.1);
        w->from_json(j);
        w->set_multiplier(j.at("multiplier").get<double>());
        return w;
    } else if (role == "Бригадир") {
        auto w = std::make_unique<Foreman>("", 0);
        w->from_json(j);
        return w;
    }
    throw std::runtime_error("Unknown worker role: " + role);
}
