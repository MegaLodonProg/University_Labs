#pragma once

#include <memory>
#include <thread>
#include <vector>
#include "Worker.hpp"
#include "Foreman.hpp"
#include "WorkZone.hpp"
#include "HashTable.hpp"
#include "../../third_party/json.hpp"

using json = nlohmann::json;

/**
 * @class Brigade
 * @brief Основной класс, управляющий бригадой работников и участками работ
 *
 * Класс содержит всю логику управления бригадой:
 * - Найм и увольнение работников
 * - Управление участками работ
 * - Расчет выработки и моделирование рабочего дня
 * - Управление отношениями между работниками
 */
class Brigade {
    HashTable<boost::uuids::uuid, std::unique_ptr<Worker>> all_workers_;
    HashTable<boost::uuids::uuid, std::unique_ptr<WorkZone>> work_zones_;
    Foreman* foreman_ptr_ = nullptr;

public:
    /**
     * @brief Конструктор по умолчанию
     */
    Brigade() : foreman_ptr_(nullptr) {};

    /**
     * @brief Конструктор с назначением бригадира
     * @param foreman Указатель на бригадира
     * @throws std::invalid_argument Если передан нулевой указатель
     */
    Brigade(std::unique_ptr<Foreman> foreman);

    /**
     * @brief Нанять работника в бригаду
     * @param worker Указатель на работника для найма
     * @throws std::invalid_argument Если:
     *         - передан нулевой указатель
     *         - работник уже существует в бригаде
     *         - пытается нанять второго бригадира
     */
    void hire_worker(std::unique_ptr<Worker> worker);

    /**
     * @brief Уволить работника из бригады
     * @param worker_id UUID увольняемого работника
     * @throws std::invalid_argument Если работник не найден
     */
    void fire_worker(const boost::uuids::uuid& worker_id);

    /**
     * @brief Направить работника на участок работ
     * @param worker_id UUID работника
     * @param zone_id UUID участка работ
     * @throws std::invalid_argument Если работник или участок не найдены
     */
    void send_worker_to_zone(const boost::uuids::uuid& worker_id, const boost::uuids::uuid& zone_id);

    /**
     * @brief Получить всех работников в бригаде
     * @return Вектор с указателями на работников
     */
    std::vector<Worker*> get_workers() const;

    /**
     * @brief Повысить рабочего до мастера
     * @param workman_id UUID рабочего для повышения
     * @param new_multiplier Новое значение множителя для мастера
     * @throws std::invalid_argument Если:
     *         - работник не найден
     *         - работник не является рабочим
     *         - уже является мастером
     */
    void promote_workman_to_master(const boost::uuids::uuid& workman_id, double new_multiplier);

    /**
     * @brief Создать новый участок работ
     * @param work_volume Объем работы на участке
     * @return UUID созданного участка работ
     */
    boost::uuids::uuid create_work_zone(double work_volume);

    /**
     * @brief Изменить объем работы на участке
     * @param zone_id UUID участка работ
     * @param new_total_work Новый общий объем работы
     * @throws std::invalid_argument Если участок не найден
     */
    void modify_work_zone(const boost::uuids::uuid& zone_id, double new_total_work);

    /**
     * @brief Добавить дружеские отношения между работниками
     * @param worker1_id UUID первого работника
     * @param worker2_id UUID второго работника
     * @throws std::invalid_argument Если один из работников не найден
     */
    void add_friendship(const boost::uuids::uuid& worker1_id, const boost::uuids::uuid& worker2_id);

    /**
     * @brief Добавить враждебные отношения между работниками
     * @param worker1_id UUID первого работника
     * @param worker2_id UUID второго работника
     * @throws std::invalid_argument Если один из работников не найден
     */
    void add_enmity(const boost::uuids::uuid& worker1_id, const boost::uuids::uuid& worker2_id);

    /**
     * @brief Получить итератор на начало контейнера участков работ
     * @return Итератор на первый элемент хэш-таблицы участков
     */
    auto get_work_zones_begin() const { return work_zones_.begin(); }

    /**
     * @brief Получить итератор на конец контейнера участков работ
     * @return Итератор на элемент после последнего в хэш-таблице
     */
    auto get_work_zones_end() const { return work_zones_.end(); }

    /**
     * @brief Смоделировать рабочий день
     * @details Применяет выработку ко всем участкам и добавляет случайные отношения
     */
    void simulate_work_day();

    /**
     * @brief Смоделировать рабочий день (многопоточная версия)
     * @param num_threads количество потоков
     * @details Каждый поток обрабатывает свою порцию участков
     */
    void simulate_work_day_parallel(size_t num_threads = std::thread::hardware_concurrency());

    /**
     * @brief Рассчитать производительность на участке
     * @param zone_id UUID участка работ
     * @return Рассчитанная производительность с учетом всех факторов
     * @throws std::invalid_argument Если участок не найден
     */
    double calculate_zone_productivity(const boost::uuids::uuid& zone_id) const;

    /**
     * @brief Получить самый проблемный участок
     * @return UUID участка с наибольшим количеством дней до завершения
     * @throws std::runtime_error Если нет участков работ
     */
    boost::uuids::uuid get_most_problematic_zone() const;

    /**
     * @brief Получить количество дней до завершения работ на участке
     * @param zone_id UUID участка работ
     * @return Количество дней до завершения
     * @throws std::invalid_argument Если участок не найден
     */
    size_t get_zone_days_remaining(const boost::uuids::uuid& zone_id) const;

    /**
     * @brief Проверить, будут ли все участки завершены сегодня
     * @return true если все участки будут завершены, false в противном случае
     */
    bool will_all_zones_be_completed_today() const;

    /**
     * @brief Получить работника по UUID
     * @param worker_id UUID работника
     * @return Указатель на работника
     * @throws std::invalid_argument Если работник не найден
     */
    Worker* get_worker(const boost::uuids::uuid& worker_id) const;

    /**
     * @brief Получить участок работ по UUID
     * @param zone_id UUID участка работ
     * @return Указатель на участок работ
     * @throws std::invalid_argument Если участок не найден
     */
    WorkZone* get_work_zone(const boost::uuids::uuid& zone_id) const;

    /**
     * @brief Сохранить состояние бригады в JSON файл
     * @param filename Имя файла для сохранения
     * @throws std::runtime_error Если не удалось открыть файл для записи
     */
    void save_to_file(const std::string& filename) const;

    /**
     * @brief Загрузить состояние бригады из JSON файла
     * @param filename Имя файла для загрузки
     * @throws std::runtime_error Если не удалось открыть файл для чтения
     */
    void load_from_file(const std::string& filename);

private:
    /**
     * @brief Суммировать продуктивность рабочих на участке
     * @param zone_id UUID участка работ
     * @return Суммарная продуктивность рабочих
     */
    double sum_workmen_productivity(const boost::uuids::uuid& zone_id) const;

    /**
     * @brief Применить множители мастеров на участке
     * @param zone_id UUID участка работ
     * @return Общий множитель производительности
     */
    double apply_masters_multipliers(const boost::uuids::uuid& zone_id) const;

    /**
     * @brief Подсчитать количество пар врагов на участке
     * @param zone_id UUID участка работ
     * @return Количество пар врагов
     */
    size_t count_pair_enemies(const boost::uuids::uuid& zone_id) const;

    /**
     * @brief Подсчитать количество троек друзей на участке
     * @param zone_id UUID участка работ
     * @return Количество троек друзей
     */
    size_t count_three_friends(const boost::uuids::uuid& zone_id) const;

    /**
     * @brief Подсчитать общее количество штрафных отношений на участке
     * @param zone_id UUID участка работ
     * @return Сумма пар врагов и троек друзей
     */
    size_t count_relationship(const boost::uuids::uuid& zone_id) const;

    /**
     * @brief Проверить наличие бригадира на участке
     * @param zone_id UUID участка работ
     * @return true если бригадир на участке, false в противном случае
     */
    bool zone_has_foreman(const boost::uuids::uuid& zone_id) const;

    /**
     * @brief Добавить случайные отношения на участке
     * @param zone_id UUID участка работ
     */
    void add_random_relationships(const boost::uuids::uuid& zone_id);

    /**
     * @brief Проверить существование работника
     * @param worker_id UUID работника
     * @return true если работник существует, false в противном случае
     */
    bool is_worker_exists(const boost::uuids::uuid& worker_id) const;

    /**
     * @brief Проверить существование участка работ
     * @param zone_id UUID участка работ
     * @return true если участок существует, false в противном случае
     */
    bool is_zone_exists(const boost::uuids::uuid& zone_id) const;

    /**
     * @brief Найти участок, к которому привязан работник
     * @param worker_id UUID работника
     * @return UUID участка работ или nil UUID если не найден
     */
    boost::uuids::uuid find_worker_zone(const boost::uuids::uuid& worker_id) const;
};