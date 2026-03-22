#include <gtest/gtest.h>
#include <string>
#include <memory>
#include "HashTable.hpp"
#include "Worker.hpp"
#include "Workman.hpp"
#include "Master.hpp"

// IMPORTANT ======================= BASIC FUNCTIONALITY TESTS =======================

TEST(HashTable, IntIntTable) {
    HashTable<int, int> table;
    table.insert(1, 100);
    table.insert(2, 200);

    EXPECT_EQ(table.size(), 2);
    EXPECT_EQ(table[1], 100);
    EXPECT_EQ(table[2], 200);
}

TEST(HashTable, StringStringTable) {
    HashTable<std::string, std::string> table;
    table.insert("key1", "value1");
    table.insert("key2", "value2");

    EXPECT_EQ(table.size(), 2);
    EXPECT_EQ(table["key1"], "value1");
    EXPECT_EQ(table["key2"], "value2");
}

TEST(HashTable, StringIntTable) {
    HashTable<std::string, int> table;
    table.insert("apple", 5);
    table.insert("banana", 3);

    EXPECT_EQ(table.size(), 2);
    EXPECT_EQ(table["apple"], 5);
    EXPECT_EQ(table["banana"], 3);
}

// IMPORTANT ======================= CONSTRUCTORS AND DESTRUCTOR TESTS =======================

TEST(HashTable, DefaultConstructor) {
    HashTable<int, std::string> table;
    EXPECT_EQ(table.size(), 0);
    EXPECT_TRUE(table.empty());
}

TEST(HashTable, InitialCapacityConstructor) {
    HashTable<int, std::string> table(20);
    EXPECT_EQ(table.size(), 0);
    EXPECT_TRUE(table.empty());
}

TEST(HashTable, MoveConstructor) {
    HashTable<int, std::string> table1;
    table1.insert(1, "one");
    table1.insert(2, "two");

    HashTable<int, std::string> table2 = std::move(table1);

    EXPECT_EQ(table2.size(), 2);
    EXPECT_EQ(table2[1], "one");
    EXPECT_EQ(table2[2], "two");
    EXPECT_EQ(table1.size(), 0);
    EXPECT_TRUE(table1.empty());
}

TEST(HashTable, MoveAssignment) {
    HashTable<int, std::string> table1;
    table1.insert(1, "one");

    HashTable<int, std::string> table2;
    table2 = std::move(table1);

    EXPECT_EQ(table2.size(), 1);
    EXPECT_EQ(table2[1], "one");
    EXPECT_EQ(table1.size(), 0);
}

// IMPORTANT ======================= INSERT AND OPERATOR[] TESTS =======================

TEST(HashTable, InsertNewElements) {
    HashTable<int, std::string> table;

    auto result1 = table.insert(1, "one");
    EXPECT_TRUE(result1);
    EXPECT_EQ(table.size(), 1);

    auto result2 = table.insert(2, "two");
    EXPECT_TRUE(result2);
    EXPECT_EQ(table.size(), 2);
}

TEST(HashTable, InsertDuplicateElements) {
    HashTable<int, std::string> table;

    table.insert(1, "one");
    auto result = table.insert(1, "new_one");

    EXPECT_FALSE(result);
    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table[1], "one");
}

TEST(HashTable, OperatorBracketInsert) {
    HashTable<int, std::string> table;

    table[1] = "one";
    table[2] = "two";

    EXPECT_EQ(table.size(), 2);
    EXPECT_EQ(table[1], "one");
    EXPECT_EQ(table[2], "two");
}

TEST(HashTable, OperatorBracketUpdate) {
    HashTable<int, std::string> table;

    table[1] = "one";
    table[1] = "updated_one";

    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table[1], "updated_one");
}

// IMPORTANT ======================= FIND AND ERASE TESTS =======================

TEST(HashTable, FindExistingElement) {
    HashTable<int, std::string> table;
    table.insert(1, "one");
    table.insert(2, "two");

    auto it1 = table.find(1);
    auto it2 = table.find(2);

    EXPECT_NE(it1, table.end());
    EXPECT_NE(it2, table.end());
    EXPECT_EQ(it1->second, "one");
    EXPECT_EQ(it2->second, "two");
}

TEST(HashTable, FindNonExistingElement) {
    HashTable<int, std::string> table;
    table.insert(1, "one");

    auto it = table.find(999);

    EXPECT_EQ(it, table.end());
}

TEST(HashTable, EraseExistingElement) {
    HashTable<int, std::string> table;
    table.insert(1, "one");
    table.insert(2, "two");

    bool result = table.erase(1);

    EXPECT_TRUE(result);
    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table.find(1), table.end());
    EXPECT_NE(table.find(2), table.end());
}

TEST(HashTable, EraseNonExistingElement) {
    HashTable<int, std::string> table;
    table.insert(1, "one");

    bool result = table.erase(999);

    EXPECT_FALSE(result);
    EXPECT_EQ(table.size(), 1);
}

TEST(HashTable, EraseByIterator) {
    HashTable<int, std::string> table;
    table.insert(1, "one");
    table.insert(2, "two");

    auto it = table.find(1);
    ASSERT_NE(it, table.end());

    auto next_it = table.erase(it);

    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table.find(1), table.end());
    EXPECT_NE(next_it, table.end());
}

// IMPORTANT ======================= ITERATOR TESTS =======================

TEST(HashTable, IteratorTraversal) {
    HashTable<int, std::string> table;
    table.insert(1, "one");
    table.insert(2, "two");
    table.insert(3, "three");

    size_t count = 0;
    for (auto it = table.begin(); it != table.end(); ++it) {
        ++count;
    }

    EXPECT_EQ(count, 3);
}

TEST(HashTable, RangeBasedForLoop) {
    HashTable<int, std::string> table;
    table.insert(1, "one");
    table.insert(2, "two");

    size_t count = 0;
    for (const auto& pair : table) {
        EXPECT_TRUE(pair.first == 1 || pair.first == 2);
        EXPECT_TRUE(pair.second == "one" || pair.second == "two");
        ++count;
    }

    EXPECT_EQ(count, 2);
}

TEST(HashTable, IteratorAccess) {
    HashTable<int, std::string> table;
    table.insert(42, "answer");

    auto it = table.find(42);
    ASSERT_NE(it, table.end());

    EXPECT_EQ(it->first, 42);
    EXPECT_EQ(it->second, "answer");
    EXPECT_EQ((*it).first, 42);
    EXPECT_EQ((*it).second, "answer");
}

// IMPORTANT ======================= REHASH AND COLLISION TESTS =======================

TEST(HashTable, AutoRehash) {
    HashTable<int, std::string> table(4);

    for (int i = 0; i < 10; ++i) {
        table.insert(i, "value_" + std::to_string(i));
    }

    EXPECT_EQ(table.size(), 10);

    // Проверяем что все элементы доступны после rehash
    for (int i = 0; i < 10; ++i) {
        auto it = table.find(i);
        EXPECT_NE(it, table.end());
        EXPECT_EQ(it->second, "value_" + std::to_string(i));
    }
}

TEST(HashTable, CollisionHandling) {
    HashTable<int, std::string> table(3);

    table.insert(1, "one");
    table.insert(4, "four");
    table.insert(7, "seven"); // Вероятная коллизия

    EXPECT_EQ(table.size(), 3);
    EXPECT_EQ(table[1], "one");
    EXPECT_EQ(table[4], "four");
    EXPECT_EQ(table[7], "seven");
}

// IMPORTANT ======================= UUID AND UNIQUE_PTR SPECIFIC TESTS =======================

TEST(HashTable, UUIDUniquePtrInsert) {
    HashTable<boost::uuids::uuid, std::unique_ptr<Worker>> table;

    auto worker1 = std::make_unique<Workman>("John", 25, 5);
    auto worker2 = std::make_unique<Master>("Alice", 30, 2.5);

    auto id1 = worker1->get_id();
    auto id2 = worker2->get_id();

    table.insert(id1, std::move(worker1));
    table.insert(id2, std::move(worker2));

    EXPECT_EQ(table.size(), 2);
    EXPECT_NE(table.find(id1), table.end());
    EXPECT_NE(table.find(id2), table.end());
}

TEST(HashTable, UUIDUniquePtrOperatorBracket) {
    HashTable<boost::uuids::uuid, std::unique_ptr<Worker>> table;

    auto worker = std::make_unique<Workman>("John", 25, 5);
    auto id = worker->get_id();

    table[id] = std::move(worker);

    EXPECT_EQ(table.size(), 1);

    auto it = table.find(id);
    ASSERT_NE(it, table.end());
    EXPECT_EQ(it->second->get_name(), "John");
}

TEST(HashTable, UUIDUniquePtrFindAndAccess) {
    HashTable<boost::uuids::uuid, std::unique_ptr<Worker>> table;

    auto worker = std::make_unique<Workman>("Bob", 35, 7);
    auto id = worker->get_id();
    table.insert(id, std::move(worker));

    const auto& const_table = table;
    auto const_it = const_table.find(id);
    ASSERT_NE(const_it, const_table.end());
    EXPECT_EQ(const_it->second->get_name(), "Bob");

    auto it = table.find(id);
    ASSERT_NE(it, table.end());
    EXPECT_EQ(it->second->get_age(), 35);
}

TEST(HashTable, UUIDUniquePtrErase) {
    HashTable<boost::uuids::uuid, std::unique_ptr<Worker>> table;

    auto worker1 = std::make_unique<Workman>("Worker1", 25, 5);
    auto worker2 = std::make_unique<Workman>("Worker2", 30, 6);

    auto id1 = worker1->get_id();
    auto id2 = worker2->get_id();

    table.insert(id1, std::move(worker1));
    table.insert(id2, std::move(worker2));

    bool erased = table.erase(id1);
    EXPECT_TRUE(erased);
    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table.find(id1), table.end());
    EXPECT_NE(table.find(id2), table.end());

    auto it = table.find(id2);
    ASSERT_NE(it, table.end());
    table.erase(it);

    EXPECT_EQ(table.size(), 0);
    EXPECT_TRUE(table.empty());
}

TEST(HashTable, UUIDUniquePtrIterator) {
    HashTable<boost::uuids::uuid, std::unique_ptr<Worker>> table;

    auto worker1 = std::make_unique<Workman>("John", 25, 5);
    auto worker2 = std::make_unique<Master>("Alice", 30, 2.5);

    auto id1 = worker1->get_id();
    auto id2 = worker2->get_id();

    table.insert(id1, std::move(worker1));
    table.insert(id2, std::move(worker2));

    // Range-based for with unique_ptr
    size_t count = 0;
    for (const auto& pair : table) {
        EXPECT_TRUE(pair.first == id1 || pair.first == id2);
        EXPECT_NE(pair.second, nullptr);
        ++count;
    }
    EXPECT_EQ(count, 2);

    count = 0;
    for (auto it = table.begin(); it != table.end(); ++it) {
        EXPECT_NE(it->second, nullptr);
        ++count;
    }
    EXPECT_EQ(count, 2);
}

TEST(HashTable, UUIDUniquePtrRehash) {
    HashTable<boost::uuids::uuid, std::unique_ptr<Worker>> table(2);

    std::vector<boost::uuids::uuid> ids;

    for (int i = 0; i < 5; ++i) {
        auto worker = std::make_unique<Workman>("Worker" + std::to_string(i), 20 + i, 5 + i);
        auto id = worker->get_id();
        ids.push_back(id);
        table.insert(id, std::move(worker));
    }

    EXPECT_EQ(table.size(), 5);

    for (const auto& id : ids) {
        auto it = table.find(id);
        ASSERT_NE(it, table.end());
        EXPECT_NE(it->second, nullptr);
    }
}

// IMPORTANT ======================= EDGE CASES AND ERROR HANDLING =======================

TEST(HashTable, EmptyTableOperations) {
    HashTable<int, std::string> table;

    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.size(), 0);
    EXPECT_EQ(table.find(1), table.end());
    EXPECT_FALSE(table.erase(1));

    // Итераторы пустой таблицы
    EXPECT_EQ(table.begin(), table.end());
}

TEST(HashTable, ClearAndReuse) {
    HashTable<int, std::string> table;

    table.insert(1, "one");
    table.insert(2, "two");

    table.erase(1);
    table.erase(2);

    EXPECT_TRUE(table.empty());

    table.insert(3, "three");
    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table[3], "three");
}

TEST(HashTable, ConstMethods) {
    HashTable<int, std::string> table;
    table.insert(1, "one");
    table.insert(2, "two");

    const auto& const_table = table;

    EXPECT_EQ(const_table.size(), 2);
    EXPECT_FALSE(const_table.empty());

    auto it = const_table.find(1);
    EXPECT_NE(it, const_table.end());
    EXPECT_EQ(it->second, "one");

    size_t count = 0;
    for (auto cit = const_table.begin(); cit != const_table.end(); ++cit) {
        ++count;
    }
    EXPECT_EQ(count, 2);
}