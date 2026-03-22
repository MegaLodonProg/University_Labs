#pragma once
#include <boost/uuid/uuid.hpp>
#include "HashTable.hpp"

template<typename Key, typename Value>
HashTable<Key, Value>::HashTable(size_t init_capacity)
    : size_(0), capacity_(init_capacity), table_(new Cell[capacity_]) {}

template<typename Key, typename Value>
HashTable<Key, Value>::HashTable(HashTable&& other) noexcept
    : size_(other.size_), capacity_(other.capacity_), table_(other.table_) {
        other.size_ = 0;
        other.capacity_ = 0;
        other.table_ = nullptr;
}

template<typename Key, typename Value>
HashTable<Key, Value>& HashTable<Key, Value>::operator=(HashTable&& other) noexcept{
    if (this != &other) {
        delete[] table_;

        size_ = other.size_;
        capacity_ = other.capacity_;
        table_ = other.table_;

        other.size_ = 0;
        other.capacity_ = 0;
        other.table_ = nullptr;
    }
    return *this;
}

template<typename Key, typename Value>
size_t HashTable<Key, Value>::hash(const Key& key) const {
    // if constexpr решается на этапе компиляции - ненужная ветка вообще не компилируется (не существует в бинарном коде)
    if constexpr (std::is_same_v<Key, boost::uuids::uuid>) {
        // Эта ветка компилируется ТОЛЬКО если Key == uuid
        // reinterpret_cast - интерпретируем байты UUID как массив из двух 64-битных чисел
        const uint64_t* data = reinterpret_cast<const uint64_t*>(key.data);
        return data[0] ^ data[1];
    }
    else {
        return std::hash<Key>{}(key);
    }
}

template<typename Key, typename Value>
size_t HashTable<Key, Value>::get_index(const Key& key) const{
    return hash(key) % capacity_;
}

template<typename Key, typename Value>
size_t HashTable<Key, Value>::probe(const size_t index, const size_t attempt) const {
    return (index + attempt) % capacity_;
}

// Находит либо элемент, либо место для вставки
template<typename Key, typename Value>
HashTable<Key, Value>::Cell* HashTable<Key, Value>::find_cell(const Key& key) {
    if (capacity_ == 0) return nullptr;

    const size_t index = get_index(key);

    for (size_t attempt = 0; attempt < capacity_; attempt++) {
        size_t current_index = probe(index, attempt);
        Cell* cell = &table_[current_index];

        if (cell->status == CellStatus::BUSY && cell->key == key) {
            return cell;
        }
        if (cell->status == CellStatus::EMPTY) {
            return cell;
        }
    }

    return nullptr; // Таблица переполнена
}

template<typename Key, typename Value>
bool HashTable<Key, Value>::insert(const Key& key, Value value) {
    if (size_ >= capacity_ * max_load_factor) {
        rehash();
    }

    Cell* cell = find_cell(key);
    if (!cell) {
        throw std::runtime_error("HashTable is full");
    }

    if (cell->status == CellStatus::BUSY) {
        return false;
    }

    cell->status = CellStatus::BUSY;
    cell->key = key;
    cell->value = std::move(value);
    size_++;

    return true;
}

template<typename Key, typename Value>
HashTable<Key, Value>::Iterator HashTable<Key, Value>::find(const Key& key) const{
    if (capacity_ == 0) return end();

    const size_t index = get_index(key);

    for (size_t attempt = 0; attempt < capacity_; attempt++) {
        size_t current_index = probe(index, attempt);
        Cell* cell = &table_[current_index];

        if (cell->status == CellStatus::EMPTY) {
            break;
        }
        if (cell->status == CellStatus::BUSY && cell->key == key) {
            return Iterator(cell, table_ + capacity_);
        }
    }

    return end();
}

template<typename Key, typename Value>
HashTable<Key, Value>::Iterator HashTable<Key, Value>::find(const Key& key) {
    if (capacity_ == 0) return end();

    const size_t index = get_index(key);

    for (size_t attempt = 0; attempt < capacity_; attempt++) {
        size_t current_index = probe(index, attempt);
        Cell* cell = &table_[current_index];

        if (cell->status == CellStatus::EMPTY) {
            break;
        }
        if (cell->status == CellStatus::BUSY && cell->key == key) {
            return Iterator(cell, table_ + capacity_);
        }
    }

    return end();
}

template<typename Key, typename Value>
bool HashTable<Key, Value>::erase(const Key& key) {
    Cell* cell = find_cell(key);
    if (cell && cell->status == CellStatus::BUSY && cell->key == key) {
        cell->status = CellStatus::DELETED;
        --size_;
        return true;
    }
    return false;
}

template<typename Key, typename Value>
HashTable<Key, Value>::Iterator HashTable<Key, Value>::erase(Iterator pos) {
    if (pos.current_ >= table_ && pos.current_ < table_ + capacity_) {
        pos.current_->status = CellStatus::DELETED;
        --size_;
    }
    auto next = pos;
    return ++next;
}

template<typename Key, typename Value>
Value& HashTable<Key, Value>::operator[](const Key& key) {
    if (size_ >= capacity_ * max_load_factor) {
        rehash();
    }

    Cell* cell = find_cell(key);
    if (!cell) {
        throw std::runtime_error("HashTable is full");
    }

    if (cell->status != CellStatus::BUSY) {
        cell->status = CellStatus::BUSY;
        cell->key = key;
        cell->value = Value {};
        size_++;
    }

    return cell->value;
}

template<typename Key, typename Value>
void HashTable<Key, Value>::clear() {
    if (table_ != nullptr) {
        for (size_t i = 0; i < capacity_; ++i) {
            table_[i].status = CellStatus::EMPTY;
        }
    }
    size_ = 0;
}

template<typename Key, typename Value>
const Value& HashTable<Key, Value>::operator[](const Key& key) const{
    auto it = find(key);
    if (it == end()) {
        throw std::out_of_range("Key not found");
    }
    return it->second;
}

template<typename Key, typename Value>
void HashTable<Key, Value>::rehash() {
    if (capacity_ == 0) {
        capacity_ = 10;
        table_ = new Cell[capacity_];
        return;
    }

    const size_t old_capacity = capacity_;
    Cell* old_table = table_;

    capacity_ *= 2;
    table_ = new Cell[capacity_];
    size_ = 0;

    for (size_t i = 0; i < old_capacity; ++i) {
        if (old_table[i].status == CellStatus::BUSY) {
            insert(old_table[i].key, std::move(old_table[i].value));
        }
    }

    delete[] old_table;
}