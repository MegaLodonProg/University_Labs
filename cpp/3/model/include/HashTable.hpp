#pragma once

#include <iostream>

/**
 * @class HashTable
 * @brief Шаблонный класс хеш-таблицы с открытой адресацией
 *
 * @tparam Key Тип ключа элементов
 * @tparam Value Тип значения элементов
 *
 * Реализует хеш-таблицу с методом открытой адресации и линейным пробированием.
 * Поддерживает основные операции: вставка, поиск, удаление, итерация.
 * Автоматически выполняет рехеширование при достижении максимального коэффициента заполнения.
 *
 * @details Использует три состояния ячеек: EMPTY, DELETED, BUSY. Поддерживает forward-итераторы
 *          для обхода только занятых ячеек.
 */
template<typename Key, typename Value>
class HashTable {

    enum class CellStatus {
        EMPTY,
        DELETED,
        BUSY
    };

    struct Cell {
        Key key;
        Value value;
        CellStatus status;

        Cell() : status(CellStatus::EMPTY) {}
    };

    size_t size_ {};
    size_t capacity_ {};
    Cell* table_ = nullptr;
    const double max_load_factor = 0.75; // максимальная заполненность для рехеширования

public:
    /**
     * @class Iterator
     * @brief Forward-итератор для обхода занятых ячеек хеш-таблицы
     */
    class Iterator {
        Cell* current_;
        Cell* table_end_;

        friend class HashTable<Key, Value>;

        /**
         * @brief Пропустить пустые и удаленные ячейки
         */
        void skip_empty_cells() {
            while (current_ != table_end_ && current_->status != CellStatus::BUSY) {
                ++current_;
            }
        }

    public:
        using iterator_category = std::forward_iterator_tag; // это forward iterator (только ++)
        using value_type = std::pair<const Key, Value>; // храним пары ключ-значение

        /**
         * @brief Конструктор итератора
         * @param start Начальная ячейка
         * @param end Конечная ячейка (за последним элементом)
         */
        Iterator(Cell* start, Cell* end) : current_(start), table_end_(end) {
            skip_empty_cells(); // Сразу переходим к первому занятому элементу
        }

        /**
         * @brief Оператор разыменования
         * @return Ссылка на пару ключ-значение
         */
        value_type& operator*() const {
            return *reinterpret_cast<value_type*>(&current_->key);
        }

        /**
         * @brief Оператор доступа к членам
         * @return Указатель на пару ключ-значение
         */
        value_type* operator->() const {
            return reinterpret_cast<value_type*>(&current_->key);
        }

        /**
         * @brief Префиксный инкремент
         * @return Ссылка на увеличенный итератор
         */
        Iterator& operator++() {
            ++current_;
            skip_empty_cells();
            return *this;
        }

        /**
         * @brief Постфиксный инкремент
         * @return Копия итератора до увеличения
         */
        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        /**
         * @brief Оператор сравнения на равенство
         * @param other Другой итератор для сравнения
         * @return true если итераторы указывают на одну ячейку
         */
        bool operator==(const Iterator& other) const {
            return current_ == other.current_;
        }

        /**
         * @brief Оператор сравнения на неравенство
         * @param other Другой итератор для сравнения
         * @return true если итераторы указывают на разные ячейки
         */
        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
    };

    /**
     * @brief Получить итератор на начало таблицы
     * @return Итератор на первый занятый элемент
     */
    Iterator begin() const {
        return Iterator(table_, table_ + capacity_);
    }

    /**
     * @brief Получить итератор на конец таблицы
     * @return Итератор за последним элементом таблицы
     */
    Iterator end() const {
        return Iterator(table_ + capacity_, table_ + capacity_);
    }

    /**
     * @brief Конструктор хеш-таблицы
     * @param init_capacity Начальная емкость таблицы
     */
    HashTable(size_t init_capacity = 10);
    HashTable(const HashTable& other) = delete;
    HashTable(HashTable&& other) noexcept;
    ~HashTable() {delete[] table_;}

    /**
     * @brief Получить емкость таблицы
     * @return Емкость таблицы
     */
    size_t debug_get_capacity() const {return capacity_;}

    /**
     * @brief Получить указатель на таблицу
     * @return Указатель на массив ячеек
     */
    Cell* get_table() const {return table_;}

    HashTable& operator=(const HashTable& other) = delete;
    HashTable& operator=(HashTable&& other) noexcept;

    /**
     * @brief Оператор доступа по ключу (неконстантный)
     * @param key Ключ для поиска
     * @return Ссылка на значение, связанное с ключом
     * @throws std::runtime_error Если таблица переполнена
     * @details Создает новый элемент, если ключ не найден
     */
    Value& operator[](const Key& key);

    /**
     * @brief Оператор доступа по ключу (константный)
     * @param key Ключ для поиска
     * @return Константная ссылка на значение, связанное с ключом
     * @throws std::out_of_range Если ключ не найден
     */
    const Value& operator[](const Key& key) const;

    /**
     * @brief Вставить элемент в таблицу
     * @param key Ключ элемента
     * @param value Значение элемента
     * @return true если элемент вставлен, false если ключ уже существует
     * @throws std::runtime_error Если таблица переполнена
     */
    bool insert(const Key& key, Value value);

    /**
     * @brief Найти элемент по ключу (константная версия)
     * @param key Ключ для поиска
     * @return Итератор на найденный элемент или end() если не найден
     */
    Iterator find(const Key& key) const;

    /**
     * @brief Найти элемент по ключу (неконстантная версия)
     * @param key Ключ для поиска
     * @return Итератор на найденный элемент или end() если не найден
     */
    Iterator find(const Key& key);

    /**
     * @brief Удалить элемент по ключу
     * @param key Ключ элемента для удаления
     * @return true если элемент удален, false если ключ не найден
     */
    bool erase(const Key& key);

    /**
     * @brief Удалить элемент по итератору
     * @param pos Итератор на элемент для удаления
     * @return Итератор на следующий элемент после удаленного
     */
    Iterator erase(Iterator pos);

    /**
     * @brief Получить количество элементов в таблице
     * @return Количество элементов
     */
    size_t size() const { return size_; }

    /**
     * @brief Проверить пустоту таблицы
     * @return true если таблица пуста, false в противном случае
     */
    bool empty() const { return size_ == 0; }

    /**
     * @brief Полная очистка таблицы
     */
    void clear();

private:
    /**
     * @brief Вычислить хеш ключа
     * @param key Ключ для хеширования
     * @return Хеш-значение ключа
     */
    size_t hash(const Key& key) const;

    /**
     * @brief Получить начальный индекс для ключа
     * @param key Ключ для поиска индекса
     * @return Начальный индекс в таблице
     */
    size_t get_index(const Key& key) const;

    /**
     * @brief Выполнить пробирование для разрешения коллизий
     * @param index Начальный индекс
     * @param attempt Номер попытки пробирования
     * @return Новый индекс после пробирования
     */
    size_t probe(size_t index, size_t attempt) const;

    /**
     * @brief Найти ячейку для ключа
     * @param key Ключ для поиска
     * @return Указатель на ячейку или nullptr если таблица переполнена
     */
    Cell* find_cell(const Key& key);

    /**
     * @brief Выполнить рехеширование таблицы
     * @details Увеличивает емкость в 2 раза и перераспределяет элементы
     */
    void rehash();
};

#include "HashTable.tpp"