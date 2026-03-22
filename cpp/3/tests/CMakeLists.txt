add_executable(test_hash_table test_hash_table.cpp)
target_include_directories(test_hash_table PRIVATE include)
target_link_libraries(test_hash_table PRIVATE CoreLib GTest::gtest_main Boost::headers)
gtest_discover_tests(test_hash_table)

# WorkZone tests
add_executable(test_workzone test_workzone.cpp)
target_include_directories(test_workzone PRIVATE include)
target_link_libraries(test_workzone PRIVATE CoreLib GTest::gtest_main Boost::headers)
gtest_discover_tests(test_workzone)

# Worker tests
add_executable(test_worker test_worker.cpp)
target_include_directories(test_worker PRIVATE include)
target_link_libraries(test_worker PRIVATE CoreLib GTest::gtest_main Boost::headers)
gtest_discover_tests(test_worker)

# Workman tests
add_executable(test_workman test_workman.cpp)
target_include_directories(test_workman PRIVATE include)
target_link_libraries(test_workman PRIVATE CoreLib GTest::gtest_main Boost::headers)
gtest_discover_tests(test_workman)

# Master tests
add_executable(test_master test_master.cpp)
target_include_directories(test_master PRIVATE include)
target_link_libraries(test_master PRIVATE CoreLib GTest::gtest_main Boost::headers)
gtest_discover_tests(test_master)

# Brigade tests
add_executable(test_brigade test_brigade.cpp)
target_include_directories(test_brigade PRIVATE include)
target_link_libraries(test_brigade PRIVATE CoreLib GTest::gtest_main Boost::headers)
gtest_discover_tests(test_brigade)