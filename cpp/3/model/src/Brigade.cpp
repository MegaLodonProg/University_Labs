#include <random>
#include <algorithm>
#include <thread>
#include <vector>
#include <boost/uuid/string_generator.hpp>
#include <fstream>
#include "Brigade.hpp"

#include "Worker.hpp"
#include "Workman.hpp"
#include "Master.hpp"
#include "WorkZone.hpp"

using namespace boost::uuids;

Brigade::Brigade(std::unique_ptr<Foreman> foreman) : foreman_ptr_(nullptr) {
    if (!foreman)
        throw std::invalid_argument("Foreman cannot be null");
    const uuid foreman_id = foreman->get_id();
    all_workers_[foreman_id] = std::move(foreman);
    foreman_ptr_ = dynamic_cast<Foreman *>(all_workers_[foreman_id].get());
}


void Brigade::hire_worker(std::unique_ptr<Worker> worker) {
    if (!worker)
        throw std::invalid_argument("Worker cannot be null");

    const uuid worker_id = worker->get_id();
    if (all_workers_.find(worker_id) != all_workers_.end()) {
        throw std::invalid_argument("Worker already exists");
    }

    auto foreman_candidate = dynamic_cast<Foreman*>(worker.get());
    if (foreman_candidate  != nullptr) {
        if (foreman_ptr_ != nullptr) {
            throw std::invalid_argument("Foreman already exists");
        }
        foreman_ptr_ = foreman_candidate;
    }

    all_workers_[worker_id] = std::move(worker);
}

void Brigade::fire_worker(const uuid& worker_id) {
    const auto it = all_workers_.find(worker_id);
    if (it == all_workers_.end()) {
        throw std::invalid_argument("Worker not found");
    }

    for (auto& [zone_id, zone_ptr] : work_zones_) {
        if (zone_ptr->has_worker(worker_id)) {
            zone_ptr->remove_worker(worker_id);
        }
    }

    if (foreman_ptr_ && foreman_ptr_->get_id() == worker_id) {
        foreman_ptr_ = nullptr;
    }

    all_workers_.erase(it);
}

bool Brigade::is_worker_exists(const uuid& worker_id) const {
    return all_workers_.find(worker_id) != all_workers_.end();
}

bool Brigade::is_zone_exists(const uuid& zone_id) const {
    return work_zones_.find(zone_id) != work_zones_.end();
}

Worker* Brigade::get_worker(const uuid& worker_id) const {
    const auto it = all_workers_.find(worker_id);
    if (it == all_workers_.end()) {
        throw std::invalid_argument("Worker not found");
    }
    return it->second.get();
}

WorkZone* Brigade::get_work_zone(const uuid& zone_id) const {
    auto it = work_zones_.find(zone_id);
    if (it == work_zones_.end()) {
        throw std::invalid_argument("Zone not found");
    }
    return it->second.get();
}

void Brigade::send_worker_to_zone(const uuid& worker_id, const uuid& zone_id) {
    if (!is_worker_exists(worker_id)) {
        throw std::invalid_argument("Worker not found");
    }
    WorkZone* target_zone = get_work_zone(zone_id);

    for (auto& [zone_uuid, zone_ptr] : work_zones_) {
        if (zone_ptr->has_worker(worker_id)) {
            zone_ptr->remove_worker(worker_id);
        }
    }

    target_zone->add_worker(worker_id);
}

uuid Brigade::find_worker_zone(const uuid& worker_id) const {
    for (const auto& [zone_id, zone_ptr] : work_zones_) {
        if (zone_ptr->has_worker(worker_id)) {
            return zone_id;
        }
    }
    return {};
}


void Brigade::promote_workman_to_master(const uuid& workman_id, double new_multiplier) {
    Worker* worker = get_worker(workman_id);

    auto workman = dynamic_cast<Workman*>(worker);
    if (!workman) {
        throw std::invalid_argument("Worker is not a Workman or is already promoted");
    }

    const uuid old_zone_id = find_worker_zone(workman_id);

    auto new_master = std::make_unique<Master>(
        workman->get_name(),
        workman->get_age(),
        new_multiplier
    );

    new_master->set_id(workman_id);

    for (const auto& friend_id : workman->get_friends()) {
        new_master->add_friend(friend_id);
    }
    for (const auto& enemy_id : workman->get_enemies()) {
        new_master->add_enemy(enemy_id);
    }

    fire_worker(workman_id);
    hire_worker(std::move(new_master));

    // отправить на зону, где был, если был где-то прикреплен
    if (!old_zone_id.is_nil()) {
        send_worker_to_zone(workman_id, old_zone_id);
    }

}

uuid Brigade::create_work_zone(double work_volume) {
    auto new_zone = std::make_unique<WorkZone>(work_volume);
    const uuid zone_id = new_zone->get_id();
    work_zones_[zone_id] = std::move(new_zone);
    return zone_id;
}

void Brigade::modify_work_zone(const uuid& zone_id, double new_total_work) {
    WorkZone* target_zone = get_work_zone(zone_id);
    target_zone->set_total_work(new_total_work);
}


double Brigade::calculate_zone_productivity(const uuid& zone_id) const {
    const double workers_sum = sum_workmen_productivity(zone_id);
    if (workers_sum <= 0) return 0.0;

    const double multiplier = apply_masters_multipliers(zone_id);
    const size_t penalty = count_relationship(zone_id);
    const bool is_foreman_here = zone_has_foreman(zone_id);
    return (workers_sum * multiplier) - (static_cast<int>(!is_foreman_here) * penalty);
}

size_t Brigade::get_zone_days_remaining(const uuid& zone_id) const {
    const double daily_productivity = calculate_zone_productivity(zone_id);
    const WorkZone* target_zone = get_work_zone(zone_id);
    return target_zone->get_days_remaining(daily_productivity);
}

uuid Brigade::get_most_problematic_zone() const {
    if (work_zones_.empty()) {
        throw std::runtime_error("No work zones available");
    }

    uuid most_problematic_zone_id = work_zones_.begin()->first;
    size_t max_days_remaining = 0;

    for (auto& [zone_id, zone_ptr] : work_zones_) {
        if (zone_ptr->is_completed()) continue;

        size_t days_remaining = get_zone_days_remaining(zone_id);
        if (days_remaining > max_days_remaining) {
            max_days_remaining = days_remaining;
            most_problematic_zone_id = zone_id;
        }
    }
    return most_problematic_zone_id;
}

bool Brigade::will_all_zones_be_completed_today() const {
    for (auto& [zone_id, zone_ptr] : work_zones_) {
        if (!zone_ptr->is_completed()) {
            double daily_productivity = calculate_zone_productivity(zone_id);
            if (zone_ptr->get_remaining_work() > daily_productivity) {
                return false;
            }
        }
    }
    return true;
}


void Brigade::add_random_relationships(const uuid& zone_id) {
    WorkZone* zone = get_work_zone(zone_id);
    const auto& workers = zone->get_workers();

    if (workers.size() < 2) return;

    std::vector<std::pair<uuid, uuid>> possible_pairs;
    for (size_t i = 0; i < workers.size(); ++i) {
        for (size_t j = i + 1; j < workers.size(); ++j) {
            possible_pairs.emplace_back(workers[i], workers[j]);
        }
    }

    if (possible_pairs.empty()) return;

    std::shuffle(possible_pairs.begin(), possible_pairs.end(),
                 std::mt19937(std::random_device{}()));

    for (const auto& [id1, id2] : possible_pairs) {
        try {
            add_friendship(id1, id2);
            break;
        } catch (const std::invalid_argument&) {
            continue;
        }
    }

    for (const auto& [id1, id2] : possible_pairs) {
        try {
            add_enmity(id1, id2);
            break;
        } catch (const std::invalid_argument&) {
            continue;
        }
    }
}


void Brigade::simulate_work_day() {
    for (auto& [zone_id, zone] : work_zones_) {
        if (!zone->is_completed()) {
            double productivity = calculate_zone_productivity(zone_id);
            zone->apply_daily_progress(productivity);
        }
    }
    for (auto& [zone_id, zone] : work_zones_) {
        add_random_relationships(zone_id);
    }
}

void Brigade::simulate_work_day_parallel(size_t num_threads) {
    if (num_threads == 0) num_threads = 1;
    if (work_zones_.empty()) return;

    // Собираем все zone_id в вектор
    std::vector<uuid> zone_ids;
    zone_ids.reserve(work_zones_.size());
    for (auto it = work_zones_.begin(); it != work_zones_.end(); ++it) {
        zone_ids.push_back(it->first);
    }

    size_t total = zone_ids.size();
    size_t chunk_size = (total + num_threads - 1) / num_threads;

    // хранение потоков
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    for (size_t t = 0; t < num_threads; ++t) {
        size_t start = t * chunk_size;
        size_t end = std::min(start + chunk_size, total);
        if (start >= total) break;

        // создание потока с лямбдой, которая обрабатывает свою порцию
        threads.emplace_back([this, &zone_ids, start, end]() {
            for (size_t i = start; i < end; ++i) {
                const auto& zone_id = zone_ids[i];
                WorkZone* zone = get_work_zone(zone_id);
                if (!zone->is_completed()) {
                    double productivity = calculate_zone_productivity(zone_id);
                    zone->apply_daily_progress(productivity);
                }
            }
            for (size_t i = start; i < end; ++i) {
                add_random_relationships(zone_ids[i]);
            }
        });
    }

    // основной поток ждёт завершения всех остальных
    for (auto& th : threads) {
        // проверка на то, что поток вообще был создан и запущен (защита от break)
        if (th.joinable()) th.join();
    }
}

double Brigade::sum_workmen_productivity(const uuid& zone_id) const {
    double sum = 0;
    WorkZone* target_zone = get_work_zone(zone_id);
    for (auto& worker_id : target_zone->get_workers()) {
        const auto it = all_workers_.find(worker_id);
        auto workman = dynamic_cast<Workman *>(it->second.get());
        if (workman) {
            sum += workman->get_productivity();
        }
    }
    return sum;
}

double Brigade::apply_masters_multipliers(const uuid& zone_id) const {
    double multy = 1.0;
    WorkZone* target_zone = get_work_zone(zone_id);
    for (auto& worker_id : target_zone->get_workers()) {
        auto it = all_workers_.find(worker_id);
        auto master = dynamic_cast<Master *>(it->second.get());
        if (master) {
            multy *= master->get_multiplier();
        }
    }
    return multy;
}

size_t Brigade::count_pair_enemies(const uuid& zone_id) const {
    size_t negative_sum = 0;
    WorkZone* target_zone = get_work_zone(zone_id);
    for (auto& worker_id : target_zone->get_workers()) {
        Worker* cur_worker = get_worker(worker_id);
        negative_sum += cur_worker->get_enemies().size();
    }
    return negative_sum / 2;
}

size_t Brigade::count_three_friends(const uuid& zone_id) const {
    WorkZone* zone = get_work_zone(zone_id);
    const auto& workers = zone->get_workers();

    size_t negative_sum = 0;

    for (size_t i = 0; i < workers.size(); i++) {
        Worker* worker_i = get_worker(workers[i]);

        for (size_t j = i + 1; j < workers.size(); j++) {
            Worker* worker_j = get_worker(workers[j]);

            if (!worker_i->is_friend(workers[j])) {
                continue;
            }

            for (size_t k = j + 1; k < workers.size(); k++) {
                if (worker_i->is_friend(workers[k]) && worker_j->is_friend(workers[k])) {
                    negative_sum++;
                    }
            }
        }
    }
    return negative_sum;
}

size_t Brigade::count_relationship(const uuid& zone_id) const {
    return (count_pair_enemies(zone_id) + count_three_friends(zone_id));
}

bool Brigade::zone_has_foreman(const uuid& zone_id) const {
    if (!foreman_ptr_) return false;

    WorkZone* zone = get_work_zone(zone_id);
    return zone->has_worker(foreman_ptr_->get_id());
}


void Brigade::add_friendship(const uuid& worker1_id, const uuid& worker2_id) {
    Worker* worker1 = get_worker(worker1_id);
    Worker* worker2 = get_worker(worker2_id);

    worker1->add_friend(worker2_id);
    worker2->add_friend(worker1_id);
}

void Brigade::add_enmity(const uuid& worker1_id, const uuid& worker2_id) {
    Worker* worker1 = get_worker(worker1_id);
    Worker* worker2 = get_worker(worker2_id);

    worker1->add_enemy(worker2_id);
    worker2->add_enemy(worker1_id);
}


void Brigade::save_to_file(const std::string& filename) const {
    json j;

    // Сохранение работников
    j["workers"] = json::array();
    for (auto it = all_workers_.begin(); it != all_workers_.end(); ++it) {
        json worker_j;
        it->second->to_json(worker_j);
        j["workers"].push_back(worker_j);
    }

    // Сохранение участков
    j["zones"] = json::array();
    for (auto it = work_zones_.begin(); it != work_zones_.end(); ++it) {
        json zone_j;
        it->second->to_json(zone_j);
        j["zones"].push_back(zone_j);
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл для записи: " + filename);
    }
    // setw - для отступов в JSON
    file << std::setw(4) << j << std::endl;
}

void Brigade::load_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл для чтения: " + filename);
    }

    json j;
    file >> j;

    // Очистка текущего состояния
    all_workers_.clear();
    work_zones_.clear();
    foreman_ptr_ = nullptr;

    for (const auto& zone_j : j.at("zones")) {
        auto zone = WorkZone::from_json(zone_j);
        work_zones_[zone->get_id()] = std::move(zone);
    }

    for (const auto& worker_j : j.at("workers")) {
        auto worker = Worker::from_json_static(worker_j);
        uuid id = string_generator()(worker_j.at("id").get<std::string>());
        worker->set_id(id);

        if (worker_j.at("role").get<std::string>() == "Бригадир") {
            foreman_ptr_ = dynamic_cast<Foreman*>(worker.get());
        }

        all_workers_[id] = std::move(worker);
    }
}


std::vector<Worker*> Brigade::get_workers() const {
    std::vector<Worker*> workers;
    for (auto it = all_workers_.begin(); it != all_workers_.end(); ++it) {
        workers.push_back(it->second.get());
    }
    return workers;
}
