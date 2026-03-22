#pragma once

#include <string>
#include <vector>
#include <boost/uuid/uuid.hpp>
#include "../../third_party/json.hpp"

using json = nlohmann::json;

/**
 * @class WorkZone
 * @brief Класс, представляющий участок работ для бригады
 *
 * Класс содержит информацию об участке работ:
 * - Уникальный идентификатор (UUID)
 * - Общий объем работы и оставшийся объем
 * - Список работников, назначенных на участок
 *
 * @details Каждый участок работ имеет уникальный UUID и характеризуется объемом работы.
 *          Поддерживает операции по управлению работниками и расчету прогресса выполнения работ.
 */
class WorkZone {
    boost::uuids::uuid id_;
    double total_work_ {};
    double remaining_work_ {};
    std::vector<boost::uuids::uuid> workers_;

public:
    /**
     * @brief Конструктор участка работ
     * @param total_work Общий объем работы на участке
     */
    explicit WorkZone(double total_work);
    WorkZone(const WorkZone&) = delete;
    WorkZone& operator=(const WorkZone&) = delete;
    WorkZone(WorkZone&&) = default;
    WorkZone& operator=(WorkZone&&) = default;
    ~WorkZone() = default;

    /**
     * @brief Получить UUID участка работ
     * @return Константная ссылка на UUID участка
     */
    const boost::uuids::uuid& get_id() const { return id_; }

    /**
     * @brief Получить UUID в виде строки
     * @return Строковое представление UUID участка
     */
    std::string get_id_string() const;

    /**
     * @brief Получить общий объем работы на участке
     * @return Общий объем работы
     */
    double get_total_work() const { return total_work_; }

    /**
     * @brief Получить оставшийся объем работы на участке
     * @return Оставшийся объем работы
     */
    double get_remaining_work() const { return remaining_work_; }

    /**
     * @brief Получить список работников на участке
     * @return Константная ссылка на вектор UUID работников
     */
    const std::vector<boost::uuids::uuid>& get_workers() const { return workers_; }

    /**
     * @brief Установить новый общий объем работы
     * @param value Новый общий объем работы
     * @details Оставшийся объем работы корректируется с учетом нового общего объема
     */
    void set_total_work(double value);

    /**
     * @brief Применить дневной прогресс к участку работ
     * @param productivity Объем работы, выполненный за день
     */
    void apply_daily_progress(double productivity);

    /**
     * @brief Рассчитать количество дней до завершения работ
     * @param daily_productivity Ежедневная производительность
     * @return Количество дней до завершения работ
     * @throws std::invalid_argument Если daily_productivity <= 0
     */
    size_t get_days_remaining(double daily_productivity) const;

    /**
     * @brief Проверить завершены ли работы на участке
     * @return true если работы завершены, false в противном случае
     */
    bool is_completed() const;

    /**
     * @brief Добавить работника на участок
     * @param worker_id UUID добавляемого работника
     * @throws std::invalid_argument Если работник уже находится на этом участке
     */
    void add_worker(const boost::uuids::uuid& worker_id);

    /**
     * @brief Удалить работника с участка
     * @param worker_id UUID удаляемого работника
     * @throws std::invalid_argument Если работник не найден на участке
     */
    void remove_worker(const boost::uuids::uuid& worker_id);

    /**
     * @brief Проверить наличие работника на участке
     * @param worker_id UUID проверяемого работника
     * @return true если работник находится на участке, false в противном случае
     */
    bool has_worker(const boost::uuids::uuid& worker_id) const;

    /**
     * @brief Сериализовать участок в JSON объект
     * @param j JSON объект для заполнения данными участка
     */
    void to_json(json& j) const;


    /**
     * @brief Создать участок работ из JSON объекта (восстанавливает данные)
     * @param j JSON объект с данными участка работ
     * @return unique_ptr<WorkZone> Умный указатель на созданный участок
     * @throws std::out_of_range Если отсутствуют обязательные поля JSON
     * @throws std::invalid_argument Если данные некорректны
     */
    static std::unique_ptr<WorkZone> from_json(const json& j);
};