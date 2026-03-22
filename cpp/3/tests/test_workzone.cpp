#include <gtest/gtest.h>
#include <boost/uuid/uuid_generators.hpp>
#include <memory>
#include "WorkZone.hpp"


// IMPORTANT ======================= CONSTRUCTOR TESTS =======================

TEST(WorkZone, Constructor) {
    WorkZone zone(100.0);

    EXPECT_EQ(zone.get_total_work(), 100.0);
    EXPECT_EQ(zone.get_remaining_work(), 100.0);
    EXPECT_FALSE(zone.is_completed());
    EXPECT_FALSE(zone.get_id().is_nil());
    EXPECT_TRUE(zone.get_workers().empty());
}

TEST(WorkZone, ConstructorZeroWork) {
    WorkZone zone(0.0);

    EXPECT_EQ(zone.get_total_work(), 0.0);
    EXPECT_EQ(zone.get_remaining_work(), 0.0);
    EXPECT_TRUE(zone.is_completed());
}

TEST(WorkZone, ConstructorNegativeWork) {
    WorkZone zone(-50.0);

    EXPECT_EQ(zone.get_total_work(), -50.0);
    EXPECT_EQ(zone.get_remaining_work(), -50.0);
    EXPECT_TRUE(zone.is_completed());
}

// IMPORTANT ======================= WORK MANAGEMENT TESTS =======================

TEST(WorkZone, ApplyDailyProgress) {
    WorkZone zone(100.0);

    zone.apply_daily_progress(30.0);
    EXPECT_EQ(zone.get_remaining_work(), 70.0);
    EXPECT_FALSE(zone.is_completed());

    zone.apply_daily_progress(70.0);
    EXPECT_EQ(zone.get_remaining_work(), 0.0);
    EXPECT_TRUE(zone.is_completed());
}

TEST(WorkZone, ApplyDailyProgressExceeding) {
    WorkZone zone(50.0);

    zone.apply_daily_progress(100.0);
    EXPECT_EQ(zone.get_remaining_work(), 0.0);
    EXPECT_TRUE(zone.is_completed());
}

TEST(WorkZone, SetTotalWorkIncrease) {
    WorkZone zone(100.0);
    zone.apply_daily_progress(40.0);

    zone.set_total_work(150.0);
    EXPECT_EQ(zone.get_total_work(), 150.0);
    EXPECT_EQ(zone.get_remaining_work(), 110.0);
    EXPECT_FALSE(zone.is_completed());
}

TEST(WorkZone, SetTotalWorkDecrease) {
    WorkZone zone(100.0);
    zone.apply_daily_progress(40.0);

    zone.set_total_work(80.0);
    EXPECT_EQ(zone.get_total_work(), 80.0);
    EXPECT_EQ(zone.get_remaining_work(), 40.0);
    EXPECT_FALSE(zone.is_completed());
}

TEST(WorkZone, SetTotalWorkToLessThanCompleted) {
    WorkZone zone(100.0);
    zone.apply_daily_progress(100.0);

    zone.set_total_work(50.0);
    EXPECT_EQ(zone.get_total_work(), 50.0);
    EXPECT_EQ(zone.get_remaining_work(), 0.0);
    EXPECT_TRUE(zone.is_completed());
}

// IMPORTANT ======================= DAYS REMAINING TESTS =======================

TEST(WorkZone, GetDaysRemaining) {
    WorkZone zone(100.0);

    EXPECT_EQ(zone.get_days_remaining(25.0), 4);
    EXPECT_EQ(zone.get_days_remaining(33.0), 4);
}

TEST(WorkZone, GetDaysRemainingCompleted) {
    WorkZone zone(100.0);
    zone.apply_daily_progress(100.0);

    EXPECT_EQ(zone.get_days_remaining(25.0), 0);
}

TEST(WorkZone, GetDaysRemainingInvalidProductivity) {
    WorkZone zone(100.0);

    EXPECT_THROW(zone.get_days_remaining(0.0), std::invalid_argument);
    EXPECT_THROW(zone.get_days_remaining(-5.0), std::invalid_argument);
}

// IMPORTANT ======================= WORKER MANAGEMENT TESTS =======================

TEST(WorkZone, AddWorker) {
    WorkZone zone(100.0);
    boost::uuids::uuid worker_id = boost::uuids::random_generator()();

    zone.add_worker(worker_id);

    EXPECT_EQ(zone.get_workers().size(), 1);
    EXPECT_TRUE(zone.has_worker(worker_id));
    EXPECT_EQ(zone.get_workers()[0], worker_id);
}

TEST(WorkZone, AddDuplicateWorker) {
    WorkZone zone(100.0);
    boost::uuids::uuid worker_id = boost::uuids::random_generator()();

    zone.add_worker(worker_id);
    EXPECT_THROW(zone.add_worker(worker_id), std::invalid_argument);
}

TEST(WorkZone, RemoveWorker) {
    WorkZone zone(100.0);
    boost::uuids::uuid worker_id = boost::uuids::random_generator()();

    zone.add_worker(worker_id);
    EXPECT_TRUE(zone.has_worker(worker_id));

    zone.remove_worker(worker_id);
    EXPECT_FALSE(zone.has_worker(worker_id));
    EXPECT_TRUE(zone.get_workers().empty());
}

TEST(WorkZone, RemoveNonexistentWorker) {
    WorkZone zone(100.0);
    boost::uuids::uuid worker_id = boost::uuids::random_generator()();

    EXPECT_THROW(zone.remove_worker(worker_id), std::invalid_argument);
}

TEST(WorkZone, MultipleWorkers) {
    WorkZone zone(100.0);
    boost::uuids::uuid worker1 = boost::uuids::random_generator()();
    boost::uuids::uuid worker2 = boost::uuids::random_generator()();
    boost::uuids::uuid worker3 = boost::uuids::random_generator()();

    zone.add_worker(worker1);
    zone.add_worker(worker2);
    zone.add_worker(worker3);

    EXPECT_EQ(zone.get_workers().size(), 3);
    EXPECT_TRUE(zone.has_worker(worker1));
    EXPECT_TRUE(zone.has_worker(worker2));
    EXPECT_TRUE(zone.has_worker(worker3));

    zone.remove_worker(worker2);
    EXPECT_EQ(zone.get_workers().size(), 2);
    EXPECT_TRUE(zone.has_worker(worker1));
    EXPECT_FALSE(zone.has_worker(worker2));
    EXPECT_TRUE(zone.has_worker(worker3));
}

// IMPORTANT ======================= MOVE SEMANTICS TESTS =======================

TEST(WorkZone, MoveConstructor) {
    WorkZone original(100.0);
    boost::uuids::uuid worker_id = boost::uuids::random_generator()();
    original.add_worker(worker_id);
    original.apply_daily_progress(30.0);

    boost::uuids::uuid original_id = original.get_id();
    WorkZone moved(std::move(original));

    EXPECT_EQ(moved.get_total_work(), 100.0);
    EXPECT_EQ(moved.get_remaining_work(), 70.0);
    EXPECT_EQ(moved.get_id(), original_id);
    EXPECT_TRUE(moved.has_worker(worker_id));
}

TEST(WorkZone, MoveAssignment) {
    WorkZone zone1(100.0);
    boost::uuids::uuid worker_id = boost::uuids::random_generator()();
    zone1.add_worker(worker_id);
    zone1.apply_daily_progress(30.0);

    boost::uuids::uuid zone1_id = zone1.get_id();
    WorkZone zone2(50.0);

    zone2 = std::move(zone1);

    EXPECT_EQ(zone2.get_total_work(), 100.0);
    EXPECT_EQ(zone2.get_remaining_work(), 70.0);
    EXPECT_EQ(zone2.get_id(), zone1_id);
    EXPECT_TRUE(zone2.has_worker(worker_id));
}

// IMPORTANT ======================= ID AND STRING TESTS =======================

TEST(WorkZone, GetIdString) {
    WorkZone zone(100.0);
    std::string id_string = zone.get_id_string();

    EXPECT_FALSE(id_string.empty());
    EXPECT_EQ(id_string.length(), 36);
    EXPECT_EQ(id_string[8], '-');
    EXPECT_EQ(id_string[13], '-');
    EXPECT_EQ(id_string[18], '-');
    EXPECT_EQ(id_string[23], '-');
}

// IMPORTANT ======================= COPY SEMANTICS TESTS =======================

TEST(WorkZone, CopyConstructorDeleted) {
    EXPECT_FALSE(std::is_copy_constructible<WorkZone>::value);
}

TEST(WorkZone, CopyAssignmentDeleted) {
    EXPECT_FALSE(std::is_copy_assignable<WorkZone>::value);
}

// IMPORTANT ======================= JSON SERIALIZATION TESTS =======================

TEST(WorkZone, ToJson) {
    WorkZone zone(150.0);
    boost::uuids::uuid worker1 = boost::uuids::random_generator()();
    boost::uuids::uuid worker2 = boost::uuids::random_generator()();

    zone.add_worker(worker1);
    zone.add_worker(worker2);
    zone.apply_daily_progress(50.0);

    json j;
    zone.to_json(j);

    EXPECT_EQ(j["total_work"], 150.0);
    EXPECT_EQ(j["remaining_work"], 100.0);
    EXPECT_FALSE(j["id"].is_null());
    EXPECT_EQ(j["workers"].size(), 2);
}

TEST(WorkZone, FromJson) {
    json j = {
        {"id", "12345678-1234-1234-1234-123456789abc"},
        {"total_work", 200.0},
        {"remaining_work", 50.0},
        {"workers", {"11111111-1111-1111-1111-111111111111", "22222222-2222-2222-2222-222222222222"}}
    };

    auto zone = WorkZone::from_json(j);

    EXPECT_EQ(zone->get_total_work(), 200.0);
    EXPECT_EQ(zone->get_remaining_work(), 50.0);
    EXPECT_EQ(zone->get_id_string(), "12345678-1234-1234-1234-123456789abc");
    EXPECT_EQ(zone->get_workers().size(), 2);
}

TEST(WorkZone, FromJsonMissingField) {
    json j = {
        {"total_work", 200.0},
        {"remaining_work", 50.0}
    };

    EXPECT_THROW(WorkZone::from_json(j), std::out_of_range);
}