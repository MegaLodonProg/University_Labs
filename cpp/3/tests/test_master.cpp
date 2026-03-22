#include <gtest/gtest.h>
#include <memory>
#include "Master.hpp"

// IMPORTANT ======================= CONSTRUCTOR TESTS =======================

TEST(Master, Constructor) {
    Master master("John", 35, 2.5);

    EXPECT_EQ(master.get_name(), "John");
    EXPECT_EQ(master.get_age(), 35);
    EXPECT_EQ(master.get_multiplier(), 2.5);
    EXPECT_FALSE(master.get_id().is_nil());
}

TEST(Master, ConstructorInvalidMultiplier) {
    EXPECT_THROW(Master("John", 35, 1.0), std::invalid_argument);
    EXPECT_THROW(Master("John", 35, 5.1), std::invalid_argument);
}

// IMPORTANT ======================= MULTIPLIER TESTS =======================

TEST(Master, SetMultiplier) {
    Master master("John", 35, 2.0);

    master.set_multiplier(3.5);
    EXPECT_EQ(master.get_multiplier(), 3.5);

    master.set_multiplier(2.2);
    EXPECT_EQ(master.get_multiplier(), 2.2);
}

TEST(Master, SetMultiplierInvalid) {
    Master master("John", 35, 2.5);

    EXPECT_THROW(master.set_multiplier(1.0), std::invalid_argument);
    EXPECT_THROW(master.set_multiplier(5.5), std::invalid_argument);
}

TEST(Master, MultiplierBoundaryValues) {
    Master master1("Min", 35, 1.1);
    Master master2("Max", 40, 5.0);

    EXPECT_GT(master1.get_multiplier(), 1.0);
    EXPECT_LE(master2.get_multiplier(), 5.0);
}

// IMPORTANT ======================= INHERITANCE TESTS =======================

TEST(Master, InheritanceAndPolymorphism) {
    auto master = std::make_unique<Master>("John", 35, 2.5);
    Worker* worker_ptr = master.get();

    EXPECT_EQ(worker_ptr->get_name(), "John");
    EXPECT_EQ(worker_ptr->get_age(), 35);

    Worker other("Jane", 30);
    worker_ptr->add_enemy(other.get_id());
    EXPECT_TRUE(worker_ptr->is_enemy(other.get_id()));
}

// IMPORTANT ======================= MOVE SEMANTICS TESTS =======================

TEST(Master, MoveConstructor) {
    auto master1 = std::make_unique<Master>("John", 35, 3.0);
    Worker other("Jane", 30);
    master1->add_friend(other.get_id());

    boost::uuids::uuid original_id = master1->get_id();
    Master master2(std::move(*master1));

    EXPECT_EQ(master2.get_name(), "John");
    EXPECT_EQ(master2.get_age(), 35);
    EXPECT_EQ(master2.get_multiplier(), 3.0);
    EXPECT_EQ(master2.get_id(), original_id);
    EXPECT_TRUE(master2.is_friend(other.get_id()));
}

// IMPORTANT ======================= JSON SERIALIZATION TESTS =======================

TEST(Master, ToJson) {
    Master master("John", 35, 2.5);

    json j;
    master.to_json(j);

    EXPECT_EQ(j["name"], "John");
    EXPECT_EQ(j["age"], 35);
    EXPECT_EQ(j["multiplier"], 2.5);
    EXPECT_EQ(j["role"], "Мастер");
    EXPECT_FALSE(j["id"].is_null());
}