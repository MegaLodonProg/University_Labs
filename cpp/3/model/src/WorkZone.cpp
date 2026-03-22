#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "../../third_party/json.hpp"
#include "WorkZone.hpp"

using json = nlohmann::json;
using namespace boost::uuids;

WorkZone::WorkZone(double total_work)
    : total_work_(total_work), remaining_work_(total_work) {
    static random_generator generator;
    id_ = generator();
}

std::string WorkZone::get_id_string() const {
    return to_string(id_);
}

void WorkZone::set_total_work(double value) {
    remaining_work_ = std::max(0.0, remaining_work_ + (value - total_work_));
    total_work_ = value;
}

void WorkZone::apply_daily_progress(double productivity) {
    if (productivity > remaining_work_) {
        remaining_work_ = 0.0;
    } else {
        remaining_work_ -= productivity;
    }
}

size_t WorkZone::get_days_remaining(double daily_productivity) const {
    if (daily_productivity <= 0) throw std::invalid_argument("daily_productivity must be > 0");
    if (is_completed()) return 0;
    const double days = remaining_work_ / daily_productivity;
    return static_cast<size_t>(std::ceil(days));
}

bool WorkZone::is_completed() const {
    return remaining_work_ <= 0.0;
}

void WorkZone::add_worker(const uuid& worker_id) {
    if (has_worker(worker_id)) {
        throw std::invalid_argument("Worker already in this zone");
    }
    workers_.push_back(worker_id);
}

void WorkZone::remove_worker(const uuid& worker_id) {
    const auto it = std::find(workers_.begin(), workers_.end(), worker_id);
    if (it == workers_.end()) {
        throw std::invalid_argument("Worker not found in this zone");
    }
    workers_.erase(it);
}

bool WorkZone::has_worker(const uuid& worker_id) const {
    return std::find(workers_.begin(), workers_.end(), worker_id) != workers_.end();
}

void WorkZone::to_json(json& j) const {
    j["id"] = get_id_string();
    j["total_work"] = total_work_;
    j["remaining_work"] = remaining_work_;
    j["workers"] = std::vector<std::string>();
    for (const auto& id : workers_) {
        j["workers"].push_back(to_string(id));
    }
}

std::unique_ptr<WorkZone> WorkZone::from_json(const json& j) {
    auto zone = std::make_unique<WorkZone>(j.at("total_work").get<double>());
    zone->id_ = string_generator()(j.at("id").get<std::string>());
    zone->remaining_work_ = j.at("remaining_work").get<double>();
    zone->workers_.clear();
    for (const auto& s : j.at("workers")) {
        zone->workers_.push_back(string_generator()(s.get<std::string>()));
    }
    return zone;
}



