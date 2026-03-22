#include <gtest/gtest.h>
#include <memory>
#include "Worker.hpp"
#include "Workman.hpp"
#include "Master.hpp"
#include "Foreman.hpp"

// IMPORTANT ======================= CONSTRUCTOR TESTS =======================

TEST(Worker, Constructor) {
    Worker worker("John Doe", 30);

    EXPECT_EQ(worker.get_name(), "John Doe");
    EXPECT_EQ(worker.get_age(), 30);
    EXPECT_FALSE(worker.get_id().is_nil());
    EXPECT_TRUE(worker.get_friends().empty());
    EXPECT_TRUE(worker.get_enemies().empty());
}

TEST(Worker, GetIdString) {
    Worker worker("John Doe", 30);
    std::string id_string = worker.get_id_string();

    EXPECT_FALSE(id_string.empty());
    EXPECT_EQ(id_string.length(), 36);
    EXPECT_EQ(id_string[8], '-');
    EXPECT_EQ(id_string[13], '-');
    EXPECT_EQ(id_string[18], '-');
    EXPECT_EQ(id_string[23], '-');
}

// IMPORTANT ======================= FRIENDSHIP MANAGEMENT TESTS =======================

TEST(Worker, AddFriend) {
    Worker worker1("John", 25);
    Worker worker2("Jane", 30);

    worker1.add_friend(worker2.get_id());

    EXPECT_EQ(worker1.get_friends().size(), 1);
    EXPECT_TRUE(worker1.is_friend(worker2.get_id()));
    EXPECT_FALSE(worker1.is_enemy(worker2.get_id()));
    EXPECT_EQ(worker1.get_friends()[0], worker2.get_id());
}

TEST(Worker, AddSelfAsFriend) {
    Worker worker("John", 25);

    EXPECT_THROW(worker.add_friend(worker.get_id()), std::invalid_argument);
    EXPECT_TRUE(worker.get_friends().empty());
}

TEST(Worker, AddDuplicateFriend) {
    Worker worker1("John", 25);
    Worker worker2("Jane", 30);

    worker1.add_friend(worker2.get_id());
    EXPECT_THROW(worker1.add_friend(worker2.get_id()), std::invalid_argument);
    EXPECT_EQ(worker1.get_friends().size(), 1);
}

TEST(Worker, AddFriendWhoIsEnemy) {
    Worker worker1("John", 25);
    Worker worker2("Jane", 30);

    worker1.add_enemy(worker2.get_id());
    EXPECT_THROW(worker1.add_friend(worker2.get_id()), std::invalid_argument);
    EXPECT_TRUE(worker1.get_friends().empty());
    EXPECT_EQ(worker1.get_enemies().size(), 1);
}

TEST(Worker, RemoveFriend) {
    Worker worker1("John", 25);
    Worker worker2("Jane", 30);

    worker1.add_friend(worker2.get_id());
    EXPECT_TRUE(worker1.is_friend(worker2.get_id()));

    worker1.remove_friend(worker2.get_id());
    EXPECT_FALSE(worker1.is_friend(worker2.get_id()));
    EXPECT_TRUE(worker1.get_friends().empty());
}

TEST(Worker, RemoveNonexistentFriend) {
    Worker worker1("John", 25);
    Worker worker2("Jane", 30);

    EXPECT_THROW(worker1.remove_friend(worker2.get_id()), std::invalid_argument);
}

// IMPORTANT ======================= ENEMY MANAGEMENT TESTS =======================

TEST(Worker, AddEnemy) {
    Worker worker1("John", 25);
    Worker worker2("Jane", 30);

    worker1.add_enemy(worker2.get_id());

    EXPECT_EQ(worker1.get_enemies().size(), 1);
    EXPECT_TRUE(worker1.is_enemy(worker2.get_id()));
    EXPECT_FALSE(worker1.is_friend(worker2.get_id()));
    EXPECT_EQ(worker1.get_enemies()[0], worker2.get_id());
}

TEST(Worker, AddSelfAsEnemy) {
    Worker worker("John", 25);

    EXPECT_THROW(worker.add_enemy(worker.get_id()), std::invalid_argument);
    EXPECT_TRUE(worker.get_enemies().empty());
}

TEST(Worker, AddDuplicateEnemy) {
    Worker worker1("John", 25);
    Worker worker2("Jane", 30);

    worker1.add_enemy(worker2.get_id());
    EXPECT_THROW(worker1.add_enemy(worker2.get_id()), std::invalid_argument);
    EXPECT_EQ(worker1.get_enemies().size(), 1);
}

TEST(Worker, AddEnemyWhoIsFriend) {
    Worker worker1("John", 25);
    Worker worker2("Jane", 30);

    worker1.add_friend(worker2.get_id());
    EXPECT_THROW(worker1.add_enemy(worker2.get_id()), std::invalid_argument);
    EXPECT_TRUE(worker1.get_enemies().empty());
    EXPECT_EQ(worker1.get_friends().size(), 1);
}

TEST(Worker, RemoveEnemy) {
    Worker worker1("John", 25);
    Worker worker2("Jane", 30);

    worker1.add_enemy(worker2.get_id());
    EXPECT_TRUE(worker1.is_enemy(worker2.get_id()));

    worker1.remove_enemy(worker2.get_id());
    EXPECT_FALSE(worker1.is_enemy(worker2.get_id()));
    EXPECT_TRUE(worker1.get_enemies().empty());
}

TEST(Worker, RemoveNonexistentEnemy) {
    Worker worker1("John", 25);
    Worker worker2("Jane", 30);

    EXPECT_THROW(worker1.remove_enemy(worker2.get_id()), std::invalid_argument);
}

// IMPORTANT ======================= MULTIPLE RELATIONSHIPS TESTS =======================

TEST(Worker, MultipleFriendsAndEnemies) {
    Worker worker1("John", 25);
    Worker worker2("Jane", 30);
    Worker worker3("Bob", 35);
    Worker worker4("Alice", 28);

    worker1.add_friend(worker2.get_id());
    worker1.add_friend(worker3.get_id());
    worker1.add_enemy(worker4.get_id());

    EXPECT_EQ(worker1.get_friends().size(), 2);
    EXPECT_EQ(worker1.get_enemies().size(), 1);
    EXPECT_TRUE(worker1.is_friend(worker2.get_id()));
    EXPECT_TRUE(worker1.is_friend(worker3.get_id()));
    EXPECT_TRUE(worker1.is_enemy(worker4.get_id()));
}

// IMPORTANT ======================= MOVE SEMANTICS TESTS =======================

TEST(Worker, MoveConstructor) {
    auto worker1 = std::make_unique<Worker>("John", 25);
    Worker worker2("Jane", 30);

    worker1->add_friend(worker2.get_id());
    boost::uuids::uuid original_id = worker1->get_id();
    std::string original_name = worker1->get_name();

    Worker moved(std::move(*worker1));

    EXPECT_EQ(moved.get_name(), original_name);
    EXPECT_EQ(moved.get_age(), 25);
    EXPECT_EQ(moved.get_id(), original_id);
    EXPECT_EQ(moved.get_friends().size(), 1);
    EXPECT_TRUE(moved.is_friend(worker2.get_id()));
}

TEST(Worker, MoveAssignment) {
    auto worker1 = std::make_unique<Worker>("John", 25);
    Worker worker2("Jane", 30);
    Worker worker3("Bob", 35);

    worker1->add_friend(worker2.get_id());
    boost::uuids::uuid original_id = worker1->get_id();
    std::string original_name = worker1->get_name();

    worker3 = std::move(*worker1);

    EXPECT_EQ(worker3.get_name(), original_name);
    EXPECT_EQ(worker3.get_age(), 25);
    EXPECT_EQ(worker3.get_id(), original_id);
    EXPECT_EQ(worker3.get_friends().size(), 1);
    EXPECT_TRUE(worker3.is_friend(worker2.get_id()));
}

// IMPORTANT ======================= COPY SEMANTICS TESTS =======================

TEST(Worker, CopyConstructorDeleted) {
    EXPECT_FALSE(std::is_copy_constructible<Worker>::value);
}

TEST(Worker, CopyAssignmentDeleted) {
    EXPECT_FALSE(std::is_copy_assignable<Worker>::value);
}

// IMPORTANT ======================= INHERITANCE TESTS =======================

TEST(Worker, WorkmanInheritance) {
    auto workman = std::make_unique<Workman>("John", 25, 5);

    EXPECT_EQ(workman->get_name(), "John");
    EXPECT_EQ(workman->get_age(), 25);
    EXPECT_EQ(workman->get_productivity(), 5);
    EXPECT_FALSE(workman->get_id().is_nil());

    // Test polymorphism
    Worker* worker_ptr = workman.get();
    EXPECT_EQ(worker_ptr->get_name(), "John");
    EXPECT_EQ(worker_ptr->get_age(), 25);
}

TEST(Worker, MasterInheritance) {
    auto master = std::make_unique<Master>("Jane", 30, 2.5);

    EXPECT_EQ(master->get_name(), "Jane");
    EXPECT_EQ(master->get_age(), 30);
    EXPECT_EQ(master->get_multiplier(), 2.5);
    EXPECT_FALSE(master->get_id().is_nil());

    Worker* worker_ptr = master.get();
    EXPECT_EQ(worker_ptr->get_name(), "Jane");
    EXPECT_EQ(worker_ptr->get_age(), 30);
}

TEST(Worker, ForemanInheritance) {
    auto foreman = std::make_unique<Foreman>("Bob", 40);

    EXPECT_EQ(foreman->get_name(), "Bob");
    EXPECT_EQ(foreman->get_age(), 40);
    EXPECT_FALSE(foreman->get_id().is_nil());

    Worker* worker_ptr = foreman.get();
    EXPECT_EQ(worker_ptr->get_name(), "Bob");
    EXPECT_EQ(worker_ptr->get_age(), 40);
}

// IMPORTANT ======================= RELATIONSHIP CONSISTENCY TESTS =======================

TEST(Worker, RelationshipConsistency) {
    Worker worker1("John", 25);
    Worker worker2("Jane", 30);
    Worker worker3("Bob", 35);

    worker1.add_friend(worker2.get_id());
    worker1.add_enemy(worker3.get_id());

    EXPECT_TRUE(worker1.is_friend(worker2.get_id()));
    EXPECT_FALSE(worker1.is_enemy(worker2.get_id()));

    EXPECT_TRUE(worker1.is_enemy(worker3.get_id()));
    EXPECT_FALSE(worker1.is_friend(worker3.get_id()));

    EXPECT_FALSE(worker2.is_friend(worker1.get_id()));
    EXPECT_FALSE(worker3.is_enemy(worker1.get_id()));
}

// IMPORTANT ======================= EDGE CASE TESTS =======================

TEST(Worker, ManyRelationships) {
    Worker worker("John", 25);

    for (int i = 0; i < 5; ++i) {
        Worker temp("Temp" + std::to_string(i), 20 + i);
        worker.add_friend(temp.get_id());
    }

    for (int i = 0; i < 3; ++i) {
        Worker temp("Enemy" + std::to_string(i), 30 + i);
        worker.add_enemy(temp.get_id());
    }

    EXPECT_EQ(worker.get_friends().size(), 5);
    EXPECT_EQ(worker.get_enemies().size(), 3);
}

// IMPORTANT ======================= JSON SERIALIZATION TESTS =======================

TEST(Worker, ToJson) {
    Worker worker("John Doe", 30);
    Worker friend_worker("Jane Doe", 28);
    Worker enemy_worker("Bob Smith", 35);

    worker.add_friend(friend_worker.get_id());
    worker.add_enemy(enemy_worker.get_id());

    json j;
    worker.to_json(j);

    EXPECT_EQ(j["name"], "John Doe");
    EXPECT_EQ(j["age"], 30);
    EXPECT_FALSE(j["id"].is_null());
    EXPECT_EQ(j["friends"].size(), 1);
    EXPECT_EQ(j["enemies"].size(), 1);
}

TEST(Worker, FromJson) {
    json j = {
        {"id", "12345678-1234-1234-1234-123456789abc"},
        {"name", "John Doe"},
        {"age", 30},
        {"friends", {"11111111-1111-1111-1111-111111111111"}},
        {"enemies", {"22222222-2222-2222-2222-222222222222"}}
    };

    Worker worker("", 0);
    worker.from_json(j);

    EXPECT_EQ(worker.get_name(), "John Doe");
    EXPECT_EQ(worker.get_age(), 30);
    EXPECT_EQ(worker.get_friends().size(), 1);
    EXPECT_EQ(worker.get_enemies().size(), 1);
}

TEST(Worker, FromJsonStaticWorkman) {
    json j = {
        {"id", "12345678-1234-1234-1234-123456789abc"},
        {"name", "John Doe"},
        {"age", 30},
        {"role", "Рабочий"},
        {"productivity", 7},
        {"friends", {}},
        {"enemies", {}}
    };

    auto worker = Worker::from_json_static(j);
    auto workman = dynamic_cast<Workman*>(worker.get());

    EXPECT_NE(workman, nullptr);
    EXPECT_EQ(workman->get_name(), "John Doe");
    EXPECT_EQ(workman->get_productivity(), 7);
}

TEST(Worker, FromJsonStaticMaster) {
    json j = {
        {"id", "12345678-1234-1234-1234-123456789abc"},
        {"name", "John Doe"},
        {"age", 30},
        {"role", "Мастер"},
        {"multiplier", 2.5},
        {"friends", {}},
        {"enemies", {}}
    };

    auto worker = Worker::from_json_static(j);
    auto master = dynamic_cast<Master*>(worker.get());

    EXPECT_NE(master, nullptr);
    EXPECT_EQ(master->get_name(), "John Doe");
    EXPECT_EQ(master->get_multiplier(), 2.5);
}
