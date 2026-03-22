#pragma once

#include <string>
#include <vector>
#include <boost/uuid/uuid.hpp>
#include "../../third_party/json.hpp"

using json = nlohmann::json;

/**
 * @class Worker
 * @brief Базовый класс, представляющий работника в бригаде
 *
 * Класс содержит общие характеристики всех работников:
 * - Уникальный идентификатор (UUID)
 * - Имя и возраст
 * - Списки друзей и врагов среди других работников
 *
 * @details Каждый работник имеет уникальный UUID, который генерируется при создании.
 *          Запрещено копирование работников для сохранения уникальности идентификаторов.
 *          Поддерживаются операции добавления/удаления друзей и врагов с проверкой корректности.
 */
class Worker {
    boost::uuids::uuid id_ {};
    std::string name_;
    unsigned int age_ {};
    std::vector<boost::uuids::uuid> friends_;
    std::vector<boost::uuids::uuid> enemies_;

public:

    /**
     * @brief Сериализовать работника в JSON объект
     * @param j JSON объект для заполнения данными работника
     */
    virtual void to_json(json& j) const;

    /**
     * @brief Десериализовать работника из JSON объекта
     * @param j JSON объект с данными работника
     * @throws std::out_of_range Если отсутствуют обязательные поля
     */
    virtual void from_json(const json& j);

    /**
     * @brief Создать работника нужного типа из JSON объекта
     * @param j JSON объект с данными работника
     * @return unique_ptr<Worker> указатель на созданного работника
     * @throws std::runtime_error Если неизвестная роль работника
     * @throws std::out_of_range Если отсутствуют обязательные поля
     * @note Определяет тип работника по полю "role" и создает
     *       соответствующий объект (Workman, Master или Foreman)
     */
    static std::unique_ptr<Worker> from_json_static(const json& j);

    /**
     * @brief Получить роль работника
     * @return Строку с ролью
     */
    virtual std::string get_role() const { return "Работник"; };

    /**
     * @brief Конструктор работника
     * @param name Имя работника
     * @param age Возраст работника
     */
    Worker(const std::string& name, unsigned int age);

    // Запрещаем копирование - не может быть 2 одинаковых человека (id)
    Worker(const Worker& other) = delete;
    Worker& operator=(const Worker& other) = delete;

    // перемещать можно
    Worker(Worker&& other) = default;
    Worker& operator=(Worker&& other) = default;
    virtual ~Worker() = default;

    /**
     * @brief Получить UUID работника
     * @return Константная ссылка на UUID работника
     */
    const boost::uuids::uuid& get_id() const { return id_; }

    /**
     * @brief Получить UUID в виде строки
     * @return Строковое представление UUID работника
     */
    std::string get_id_string() const;

    /**
     * @brief Получить возраст работника
     * @return Возраст работника
     */
    unsigned int get_age() const { return age_; }

    /**
     * @brief Получить имя работника
     * @return Константная ссылка на имя работника
     */
    const std::string& get_name() const { return name_; }

    /**
     * @brief Получить список друзей работника
     * @return Константная ссылка на вектор UUID друзей
     */
    const std::vector<boost::uuids::uuid>& get_friends() const { return friends_; }

    /**
     * @brief Получить список врагов работника
     * @return Константная ссылка на вектор UUID врагов
     */
    const std::vector<boost::uuids::uuid>& get_enemies() const { return enemies_; }

    /**
     * @brief Установить новый UUID работника
     * @param new_id Новый UUID
     */
    void set_id(const boost::uuids::uuid& new_id) { id_ = new_id; }

    /**
     * @brief Добавить друга
     * @param friend_id UUID работника, которого нужно добавить в друзья
     * @throws std::invalid_argument Если:
     *         - пытаемся добавить самого себя в друзья
     *         - работник уже является другом
     *         - работник уже является врагом
     */
    void add_friend(const boost::uuids::uuid& friend_id);

    /**
     * @brief Удалить друга
     * @param friend_id UUID работника, которого нужно удалить из друзей
     * @throws std::invalid_argument Если указанный работник не найден в списке друзей
     */
    void remove_friend(const boost::uuids::uuid& friend_id);

    /**
     * @brief Добавить врага
     * @param enemy_id UUID работника, которого нужно добавить во враги
     * @throws std::invalid_argument Если:
     *         - пытаемся добавить самого себя во враги
     *         - работник уже является врагом
     *         - работник уже является другом
     */
    void add_enemy(const boost::uuids::uuid& enemy_id);

    /**
     * @brief Удалить врага
     * @param enemy_id UUID работника, которого нужно удалить из врагов
     * @throws std::invalid_argument Если указанный работник не найден в списке врагов
     */
    void remove_enemy(const boost::uuids::uuid& enemy_id);

    /**
     * @brief Проверить, является ли работник другом
     * @param other_id UUID проверяемого работника
     * @return true если работник является другом, false в противном случае
     */
    bool is_friend(const boost::uuids::uuid& other_id) const;

    /**
     * @brief Проверить, является ли работник врагом
     * @param other_id UUID проверяемого работника
     * @return true если работник является врагом, false в противном случае
     */
    bool is_enemy(const boost::uuids::uuid& other_id) const;

};