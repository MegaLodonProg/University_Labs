#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <vector>
#include <filesystem>
#include "Brigade.hpp"
#include "Foreman.hpp"
#include "Workman.hpp"
#include "Master.hpp"

using Clock = std::chrono::steady_clock;

// Флаг: генерировать ли файлы заново
const bool REGENERATE_FILES = false;

// Создание бригады с заданным числом зон и сохранение в файл (если флаг true)
std::unique_ptr<Brigade> generate_brigade(size_t num_zones) {
    auto foreman = std::make_unique<Foreman>("Иван", 45);
    auto brigade = std::make_unique<Brigade>(std::move(foreman));

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> name_dist(0, 999);
    std::uniform_int_distribution<> age_dist(20, 60);
    std::uniform_int_distribution<> prod_dist(1, 10);
    std::uniform_real_distribution<> mult_dist(1.1, 2.0);
    std::uniform_real_distribution<> work_dist(1000.0, 5000.0);

    std::vector<boost::uuids::uuid> zone_ids;
    zone_ids.reserve(num_zones);

    // Создание зон
    for (size_t i = 0; i < num_zones; ++i) {
        zone_ids.push_back(brigade->create_work_zone(work_dist(rng)));
    }

    // Добавление работников
    for (size_t i = 0; i < num_zones; ++i) {
        for (int j = 0; j < 15; ++j) {
            std::string name = "Worker_" + std::to_string(name_dist(rng));
            auto w = std::make_unique<Workman>(name, age_dist(rng), prod_dist(rng));
            auto id = w->get_id();
            brigade->hire_worker(std::move(w));
            brigade->send_worker_to_zone(id, zone_ids[i]);
        }
        for (int j = 0; j < 2; ++j) {
            std::string name = "Master_" + std::to_string(name_dist(rng));
            auto m = std::make_unique<Master>(name, age_dist(rng), mult_dist(rng));
            auto id = m->get_id();
            brigade->hire_worker(std::move(m));
            brigade->send_worker_to_zone(id, zone_ids[i]);
        }
    }

    return brigade;
}

double measure_time(std::function<void()> func, int repeats = 5) {
    std::vector<double> times;
    for (int r = 0; r < repeats; ++r) {
        auto start = Clock::now();
        func();
        auto end = Clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        times.push_back(dur.count() / 1e9); // перевод в секунды
    }
    return std::accumulate(times.begin(), times.end(), 0.0) / repeats;
}

int main() {
    std::string data_dir = "data/";

    std::vector<size_t> zone_counts = {10000, 15000, 20000, 25000, 30000};

    // генерация файлов (если флаг тру)
    if (REGENERATE_FILES) {
        for (size_t n : zone_counts) {
            std::cout << "  Creating file for " << n << " zones... ";
            auto brigade = generate_brigade(n);
            std::string filename = "../" + data_dir + "brigade_" + std::to_string(n) + ".json";
            brigade->save_to_file(filename);
            std::cout << "ready (" << filename << ")\n";
        }
    }

    // сравнение двух функций
    std::ofstream out("parallel_test.txt");
    out << "zones \t single_sec \t parallel_sec \n";

    for (size_t n : zone_counts) {
        std::cout << "For " << n << " zones...\n";

        std::string filename = "../" + data_dir + "brigade_" + std::to_string(n) + ".json";

        auto brigade1 = std::make_unique<Brigade>();
        brigade1->load_from_file(filename);

        auto brigade2 = std::make_unique<Brigade>();
        brigade2->load_from_file(filename);

        double t1 = measure_time([&]() { brigade1->simulate_work_day(); });
        double t2 = measure_time([&]() { brigade2->simulate_work_day_parallel(4); });

        std::cout << "  Single: " << t1 << "s, Parallel: " << t2 << "s\n";
        out << n << "\t" << t1 << "\t" << t2 << "\n";
    }

    out.close();
    std::cout << "\nResults saved to parallel_test.txt\n";

    return 0;
}