# Тесты для StudentWork
add_executable(test_student_work test_student_work.cpp)
target_link_libraries(test_student_work PRIVATE StudentWorkLib GTest::gtest_main)
gtest_discover_tests(test_student_work)

# Тесты для WorkStack
add_executable(test_workstack test_workstack.cpp)
target_link_libraries(test_workstack PRIVATE StudentWorkLib GTest::gtest_main)
gtest_discover_tests(test_workstack)

# Тесты для MyVector
add_executable(test_myvector test_myvector.cpp)
target_link_libraries(test_myvector PRIVATE StudentWorkLib GTest::gtest_main)
gtest_discover_tests(test_myvector)

# GTest::gtest_main - уже делает стандартную функцию запуска всех тестов за меня