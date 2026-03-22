#include <gtest/gtest.h>
#include <memory>
#include "Workman.hpp"

// IMPORTANT ======================= CONSTRUCTOR TESTS =======================

TEST(Workman, Constructor) {
    Workman workman("John", 25, 5);

    EXPECT_EQ(workman.get_name(), "John");
    EXPECT_EQ(workman.get_age(), 25);
    EXPECT_EQ(workman.get_productivity(), 5);
    EXPECT_FALSE(workman.get_id().is_nil());
}

TEST(Workman, ConstructorInvalidProductivity) {
    EXPECT_THROW(Workman("John", 25, 0), std::invalid_argument);
    EXPECT_THROW(Workman("John", 25, 11), std::invalid_argument);
}

// IMPORTANT ======================= PRODUCTIVITY TESTS =======================

TEST(Workman, SetProductivity) {
    Workman workman("John", 25, 5);

    workman.set_productivity(8);
    EXPECT_EQ(workman.get_productivity(), 8);

    workman.set_productivity(3);
    EXPECT_EQ(workman.get_productivity(), 3);
}

TEST(Workman, SetProductivityInvalid) {
    Workman workman("John", 25, 5);

    EXPECT_THROW(workman.set_productivity(0), std::invalid_argument);
    EXPECT_THROW(workman.set_productivity(11), std::invalid_argument);
}

TEST(Workman, ProductivityBoundaryValues) {
    Workman workman1("Min", 25, 1);
    Workman workman2("Max", 30, 10);

    EXPECT_EQ(workman1.get_productivity(), 1);
    EXPECT_EQ(workman2.get_productivity(), 10);
}

// IMPORTANT ======================= INHERITANCE TESTS =======================

TEST(Workman, InheritanceAndPolymorphism) {
    auto workman = std::make_unique<Workman>("John", 25, 7);
    Worker* worker_ptr = workman.get();

    EXPECT_EQ(worker_ptr->get_name(), "John");
    EXPECT_EQ(worker_ptr->get_age(), 25);

    // Проверяем, что можем использовать методы Worker
    Worker other("Jane", 30);
    worker_ptr->add_friend(other.get_id());
    EXPECT_TRUE(worker_ptr->is_friend(other.get_id()));
}

// IMPORTANT ======================= MOVE SEMANTICS TESTS =======================

TEST(Workman, MoveConstructor) {
    auto workman1 = std::make_unique<Workman>("John", 25, 5);
    Worker other("Jane", 30);
    workman1->add_friend(other.get_id());

    boost::uuids::uuid original_id = workman1->get_id();
    Workman workman2(std::move(*workman1));

    EXPECT_EQ(workman2.get_name(), "John");
    EXPECT_EQ(workman2.get_age(), 25);
    EXPECT_EQ(workman2.get_productivity(), 5);
    EXPECT_EQ(workman2.get_id(), original_id);
    EXPECT_TRUE(workman2.is_friend(other.get_id()));
}

TEST(Workman, MoveAssignment) {
    auto workman1 = std::make_unique<Workman>("John", 25, 5);
    Workman workman2("Jane", 30, 8);
    Worker other("Bob", 35);
    workman1->add_friend(other.get_id());

    boost::uuids::uuid original_id = workman1->get_id();
    workman2 = std::move(*workman1);

    EXPECT_EQ(workman2.get_name(), "John");
    EXPECT_EQ(workman2.get_age(), 25);
    EXPECT_EQ(workman2.get_productivity(), 5);
    EXPECT_EQ(workman2.get_id(), original_id);
    EXPECT_TRUE(workman2.is_friend(other.get_id()));
}


// IMPORTANT ======================= JSON SERIALIZATION TESTS =======================

TEST(Workman, ToJson) {
    Workman workman("John", 25, 5);

    json j;
    workman.to_json(j);

    EXPECT_EQ(j["name"], "John");
    EXPECT_EQ(j["age"], 25);
    EXPECT_EQ(j["productivity"], 5);
    EXPECT_EQ(j["role"], "Рабочий");
    EXPECT_FALSE(j["id"].is_null());
}