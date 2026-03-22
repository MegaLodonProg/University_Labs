#include <gtest/gtest.h>
#include <boost/uuid/uuid_generators.hpp>
#include <memory>
#include "Brigade.hpp"
#include "Workman.hpp"
#include "Master.hpp"
#include "Foreman.hpp"

// IMPORTANT ======================= BASIC TESTS =======================

TEST(Brigade, ConstructorWithForeman) {
    auto foreman = std::make_unique<Foreman>("Foreman John", 45);
    auto foreman_id = foreman->get_id();
    Brigade brigade(std::move(foreman));

    EXPECT_NO_THROW(brigade.get_worker(foreman_id));
    EXPECT_EQ(brigade.get_worker(foreman_id)->get_name(), "Foreman John");
}

TEST(Brigade, HireAndFireWorker) {
    Brigade brigade;
    auto worker = std::make_unique<Workman>("John", 25, 5);
    auto worker_id = worker->get_id();

    brigade.hire_worker(std::move(worker));
    EXPECT_EQ(brigade.get_worker(worker_id)->get_name(), "John");

    brigade.fire_worker(worker_id);
    EXPECT_THROW(brigade.get_worker(worker_id), std::invalid_argument);
}

// IMPORTANT ======================= WORK ZONE TESTS =======================

TEST(Brigade, CreateWorkZone) {
    Brigade brigade;

    auto zone_id = brigade.create_work_zone(100.0);

    auto* zone = brigade.get_work_zone(zone_id);
    EXPECT_EQ(zone->get_total_work(), 100.0);
    EXPECT_FALSE(zone->is_completed());
}

TEST(Brigade, ModifyWorkZone) {
    Brigade brigade;

    auto zone_id = brigade.create_work_zone(100.0);
    brigade.modify_work_zone(zone_id, 150.0);

    auto* zone = brigade.get_work_zone(zone_id);
    EXPECT_EQ(zone->get_total_work(), 150.0);
}

// IMPORTANT ======================= ZONE ASSIGNMENT TESTS =======================

TEST(Brigade, SendWorkerToZone) {
    Brigade brigade;
    auto worker = std::make_unique<Workman>("John", 25, 5);
    auto worker_id = worker->get_id();
    brigade.hire_worker(std::move(worker));

    auto zone_id = brigade.create_work_zone(100.0);
    brigade.send_worker_to_zone(worker_id, zone_id);

    auto* zone = brigade.get_work_zone(zone_id);
    EXPECT_TRUE(zone->has_worker(worker_id));
}

// IMPORTANT ======================= PRODUCTIVITY TESTS =======================

TEST(Brigade, CalculateProductivityEmptyZone) {
    Brigade brigade;
    auto zone_id = brigade.create_work_zone(100.0);

    double productivity = brigade.calculate_zone_productivity(zone_id);
    EXPECT_EQ(productivity, 0.0);
}

TEST(Brigade, CalculateProductivityWithWorker) {
    Brigade brigade;
    auto workman = std::make_unique<Workman>("John", 25, 5);
    auto workman_id = workman->get_id();
    brigade.hire_worker(std::move(workman));

    auto zone_id = brigade.create_work_zone(100.0);
    brigade.send_worker_to_zone(workman_id, zone_id);

    double productivity = brigade.calculate_zone_productivity(zone_id);
    EXPECT_EQ(productivity, 5.0);
}

// IMPORTANT ======================= PROMOTION TESTS =======================

TEST(Brigade, PromoteWorkmanToMaster) {
    Brigade brigade;
    auto workman = std::make_unique<Workman>("John", 25, 5);
    auto workman_id = workman->get_id();
    brigade.hire_worker(std::move(workman));

    brigade.promote_workman_to_master(workman_id, 2.5);
    auto* promoted = dynamic_cast<Master*>(brigade.get_worker(workman_id));
    EXPECT_NE(promoted, nullptr);
    EXPECT_EQ(promoted->get_multiplier(), 2.5);
    EXPECT_EQ(promoted->get_name(), "John");
}

// IMPORTANT ======================= SIMULATION TESTS =======================

TEST(Brigade, SimulateWorkDayCompletesZone) {
    Brigade brigade;
    auto workman = std::make_unique<Workman>("John", 25, 10);
    auto workman_id = workman->get_id();
    brigade.hire_worker(std::move(workman));

    auto zone_id = brigade.create_work_zone(10.0);
    brigade.send_worker_to_zone(workman_id, zone_id);
    brigade.simulate_work_day();

    auto* zone = brigade.get_work_zone(zone_id);
    EXPECT_TRUE(zone->is_completed());
}

TEST(Brigade, GetMostProblematicZone) {
    Brigade brigade;
    auto zone1_id = brigade.create_work_zone(50.0);
    auto zone2_id = brigade.create_work_zone(100.0);

    auto workman = std::make_unique<Workman>("John", 25, 5);
    auto workman_id = workman->get_id();

    auto workman2 = std::make_unique<Workman>("Oleg", 21, 4);
    auto workman_id2 = workman2->get_id();

    brigade.hire_worker(std::move(workman));
    brigade.hire_worker(std::move(workman2));
    brigade.send_worker_to_zone(workman_id, zone2_id);
    brigade.send_worker_to_zone(workman_id2, zone1_id);

    auto problematic_id = brigade.get_most_problematic_zone();
    EXPECT_EQ(problematic_id, zone2_id);
}

TEST(Brigade, GetMostProblematicZoneWithNoZones) {
    Brigade brigade;
    EXPECT_THROW(brigade.get_most_problematic_zone(), std::runtime_error);
}


TEST(Brigade, HireNullWorker) {
    Brigade brigade;
    EXPECT_THROW(brigade.hire_worker(nullptr), std::invalid_argument);
}

TEST(Brigade, SendWorkerToNonexistentZone) {
    Brigade brigade;
    auto worker = std::make_unique<Workman>("John", 25, 5);
    auto worker_id = worker->get_id();
    brigade.hire_worker(std::move(worker));

    auto fake_zone_id = boost::uuids::random_generator()();
    EXPECT_THROW(brigade.send_worker_to_zone(worker_id, fake_zone_id), std::invalid_argument);
}

TEST(Brigade, CalculateProductivityWithMaster) {
    Brigade brigade;
    auto master = std::make_unique<Master>("Master John", 35, 2.0);
    auto master_id = master->get_id();
    brigade.hire_worker(std::move(master));

    auto zone_id = brigade.create_work_zone(100.0);
    brigade.send_worker_to_zone(master_id, zone_id);

    double productivity = brigade.calculate_zone_productivity(zone_id);
    EXPECT_EQ(productivity, 0.0);
}

TEST(Brigade, CalculateProductivityWithMasterAndWorkman) {
    Brigade brigade;
    auto workman = std::make_unique<Workman>("John", 25, 5);
    auto master = std::make_unique<Master>("Master Bob", 35, 2.0);
    auto workman_id = workman->get_id();
    auto master_id = master->get_id();

    brigade.hire_worker(std::move(workman));
    brigade.hire_worker(std::move(master));

    auto zone_id = brigade.create_work_zone(100.0);
    brigade.send_worker_to_zone(workman_id, zone_id);
    brigade.send_worker_to_zone(master_id, zone_id);

    double productivity = brigade.calculate_zone_productivity(zone_id);
    EXPECT_EQ(productivity, 10.0);
}

TEST(Brigade, FireWorkerFromZone) {
    Brigade brigade;
    auto worker = std::make_unique<Workman>("John", 25, 5);
    auto worker_id = worker->get_id();
    brigade.hire_worker(std::move(worker));

    auto zone_id = brigade.create_work_zone(100.0);
    brigade.send_worker_to_zone(worker_id, zone_id);

    auto* zone = brigade.get_work_zone(zone_id);
    EXPECT_TRUE(zone->has_worker(worker_id));

    brigade.fire_worker(worker_id);
    EXPECT_FALSE(zone->has_worker(worker_id));
}

TEST(Brigade, PromoteWorkmanPreservesRelationships) {
    Brigade brigade;
    auto workman = std::make_unique<Workman>("John", 25, 5);
    auto friend_worker = std::make_unique<Workman>("Jane", 30, 6);
    auto workman_id = workman->get_id();
    auto friend_id = friend_worker->get_id();

    brigade.hire_worker(std::move(workman));
    brigade.hire_worker(std::move(friend_worker));
    brigade.add_friendship(workman_id, friend_id);

    brigade.promote_workman_to_master(workman_id, 2.5);

    auto* promoted = brigade.get_worker(workman_id);
    EXPECT_TRUE(promoted->is_friend(friend_id));
}

// IMPORTANT ======================= RELATIONSHIP TESTS =======================

TEST(Brigade, AddFriendship) {
    Brigade brigade;
    auto worker1 = std::make_unique<Workman>("John", 25, 5);
    auto worker2 = std::make_unique<Workman>("Jane", 30, 6);
    auto worker1_id = worker1->get_id();
    auto worker2_id = worker2->get_id();

    brigade.hire_worker(std::move(worker1));
    brigade.hire_worker(std::move(worker2));
    brigade.add_friendship(worker1_id, worker2_id);

    auto* w1 = brigade.get_worker(worker1_id);
    auto* w2 = brigade.get_worker(worker2_id);

    EXPECT_TRUE(w1->is_friend(worker2_id));
    EXPECT_TRUE(w2->is_friend(worker1_id));
}

TEST(Brigade, AddEnmity) {
    Brigade brigade;
    auto worker1 = std::make_unique<Workman>("John", 25, 5);
    auto worker2 = std::make_unique<Workman>("Jane", 30, 6);
    auto worker1_id = worker1->get_id();
    auto worker2_id = worker2->get_id();

    brigade.hire_worker(std::move(worker1));
    brigade.hire_worker(std::move(worker2));
    brigade.add_enmity(worker1_id, worker2_id);

    auto* w1 = brigade.get_worker(worker1_id);
    auto* w2 = brigade.get_worker(worker2_id);

    EXPECT_TRUE(w1->is_enemy(worker2_id));
    EXPECT_TRUE(w2->is_enemy(worker1_id));
}

// IMPORTANT ======================= UTILITY FUNCTION TESTS =======================

TEST(Brigade, GetZoneDaysRemaining) {
    Brigade brigade;
    auto workman = std::make_unique<Workman>("John", 25, 5);
    auto workman_id = workman->get_id();

    brigade.hire_worker(std::move(workman));
    auto zone_id = brigade.create_work_zone(50.0);
    brigade.send_worker_to_zone(workman_id, zone_id);

    size_t days = brigade.get_zone_days_remaining(zone_id);
    EXPECT_EQ(days, 10);
}

// IMPORTANT ======================= FILE IO TESTS =======================

TEST(Brigade, SaveAndLoadToFile) {
    Brigade original;
    auto foreman = std::make_unique<Foreman>("Foreman Bob", 45);
    auto workman = std::make_unique<Workman>("John", 25, 5);
    auto master = std::make_unique<Master>("Jane", 30, 2.5);

    original.hire_worker(std::move(foreman));
    original.hire_worker(std::move(workman));
    original.hire_worker(std::move(master));

    original.create_work_zone(100.0);

    const std::string filename = "test_brigade.json";
    original.save_to_file(filename);

    Brigade loaded;
    loaded.load_from_file(filename);

    auto workers = loaded.get_workers();
    EXPECT_EQ(workers.size(), 3);

    std::remove(filename.c_str());
}