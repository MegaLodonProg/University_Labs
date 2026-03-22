#include <gtest/gtest.h>
#include "WorkStack.hpp"
#include "StudentWork.hpp"

// IMPORTANT ======================= CONSTRUCTORS AND BASIC OPERATIONS TESTS =======================

TEST(WorkStack, DefaultConstructor) {
    WorkStack stack;
    EXPECT_TRUE(stack.empty());
}

TEST(WorkStack, VectorConstructor) {
    MyVector<StudentWork> works;
    works.push_back(StudentWork("Ivanov", 5, 1, 3));
    works.push_back(StudentWork("Petrov", 4, 1, 2));

    WorkStack stack(works);
    EXPECT_FALSE(stack.empty());
}

TEST(WorkStack, ArrayConstructor) {
    StudentWork works[] = {
        StudentWork("Ivanov", 5, 1, 3),
        StudentWork("Petrov", 4, 1, 2)
    };

    WorkStack stack(works, 2);
    EXPECT_FALSE(stack.empty());
}

TEST(WorkStack, ConstructorEmptyVector) {
    const MyVector<StudentWork> empty_vec;
    const WorkStack stack(empty_vec);
    EXPECT_TRUE(stack.empty());
}

// IMPORTANT ======================= STACK OPERATIONS TESTS =======================

TEST(WorkStack, PushAndPop) {
    WorkStack stack;
    StudentWork work("Ivanov", 5, 1, 3);

    stack += work;
    EXPECT_FALSE(stack.empty());

    StudentWork popped = stack.pop();
    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(popped.get_surname(), "Ivanov");
}

TEST(WorkStack, PopEmpty) {
    WorkStack stack;
    EXPECT_THROW(stack.pop(), std::out_of_range);
}

TEST(WorkStack, PopWithoutGrade) {
    WorkStack stack;
    stack += StudentWork("Ivanov", std::nullopt, 1, 3);
    stack += StudentWork("Petrov", 5, 1, 2);

    StudentWork ungraded = stack.pop_without_grade();
    EXPECT_EQ(ungraded.get_surname(), "Ivanov");
    EXPECT_FALSE(ungraded.get_grade().has_value());
}

TEST(WorkStack, PopWithoutGradeNotFound) {
    WorkStack stack;
    stack += StudentWork("Ivanov", 5, 1, 3);
    stack += StudentWork("Petrov", 4, 1, 2);

    EXPECT_THROW(stack.pop_without_grade(), std::runtime_error);
}

// IMPORTANT ======================= COMPLEX OPERATIONS TESTS =======================

TEST(WorkStack, MergeFragmentedWorks) {
    WorkStack stack;
    stack += StudentWork("Ivanov", std::nullopt, 1, 2);
    stack += StudentWork("Ivanov", std::nullopt, 3, 4);
    stack += StudentWork("Petrov", 5, 1, 1);

    stack.merge_fragmented_works();
    EXPECT_FALSE(stack.empty());
    StudentWork petrov = stack.pop();
    EXPECT_EQ(petrov.get_surname(), "Ivanov");
    const StudentWork ivanov = stack.pop();
    EXPECT_EQ(ivanov.get_surname(), "Petrov");
    EXPECT_TRUE(stack.empty());
}

TEST(WorkStack, SplitAllPages) {
    WorkStack stack;
    stack += StudentWork("Ivanov", 5, 1, 3);

    WorkStack split = stack.split_all_pages();
    EXPECT_FALSE(split.empty());
}

TEST(WorkStack, MergeEmpty) {
    WorkStack stack;
    EXPECT_NO_THROW(stack.merge_fragmented_works());
}

// IMPORTANT ======================= ASSIGNMENT OPERATOR TESTS =======================

TEST(WorkStack, CopyAssignment) {
    WorkStack stack1;
    stack1 += StudentWork("Ivanov", 5, 1, 3);

    WorkStack stack2;
    stack2 = stack1;

    EXPECT_FALSE(stack2.empty());
    EXPECT_EQ(stack2.pop().get_surname(), "Ivanov");
}

TEST(WorkStack, SelfAssignment) {
    WorkStack stack;
    stack += StudentWork("Ivanov", 5, 1, 3);

    stack = stack;
    EXPECT_FALSE(stack.empty());
}

// IMPORTANT ======================= STREAM OPERATORS TESTS =======================

TEST(WorkStack, OutputOperator) {
    WorkStack stack;
    stack += StudentWork("Ivanov", 5, 1, 3);

    std::ostringstream oss;
    oss << stack;

    EXPECT_FALSE(oss.str().empty());
    EXPECT_NE(oss.str().find("Ivanov"), std::string::npos);
}

TEST(WorkStack, InputOperator) {
    std::istringstream iss("Ivanov 5 1 3\nPetrov 4 1 2");
    WorkStack stack;

    iss >> stack;
    EXPECT_FALSE(stack.empty());
}

TEST(WorkStack, IOEdgeCases) {
    // Пустой вывод
    WorkStack empty;
    std::ostringstream oss1;
    oss1 << empty;
    EXPECT_TRUE(oss1.str().empty());

    // Ошибочный ввод
    WorkStack stack;
    std::istringstream iss("Invalid\nData");
    EXPECT_THROW(iss >> stack, std::runtime_error);
}

TEST(WorkStack, InputEmptyStream) {
    WorkStack stack;

    std::istringstream iss("");

    EXPECT_THROW(iss >> stack, std::runtime_error);
    EXPECT_TRUE(stack.empty());
}

TEST(WorkStack, InputInvalidData) {
    WorkStack stack;
    std::istringstream iss("Invalid Data\nMore invalid data");

    EXPECT_THROW(iss >> stack, std::runtime_error);
}

TEST(WorkStack, OutputEmptyStack) {
    WorkStack stack;
    std::ostringstream oss;

    oss << stack;
    EXPECT_TRUE(oss.str().empty());
}

// IMPORTANT ======================= EDGE CASES AND ERROR HANDLING =======================

TEST(WorkStack, MultiplePopWithoutGrade) {
    WorkStack stack;
    stack += StudentWork("Ivanov", std::nullopt, 1, 3);
    stack += StudentWork("Petrov", std::nullopt, 1, 2);
    stack += StudentWork("Sidorov", 5, 1, 1);

    StudentWork first = stack.pop_without_grade();
    StudentWork second = stack.pop_without_grade();

    EXPECT_FALSE(first.get_grade().has_value());
    EXPECT_FALSE(second.get_grade().has_value());
    EXPECT_EQ(stack.pop().get_surname(), "Sidorov");
}

TEST(WorkStack, MergeFragmentedNonAdjacentWorks) {
    WorkStack stack;
    stack += StudentWork("Ivanov", std::nullopt, 1, 2);
    stack += StudentWork("Petrov", 4, 1, 1);
    stack += StudentWork("Ivanov", std::nullopt, 4, 5); // Пропуск страницы 3

    stack.merge_fragmented_works();

    // Должны остаться 3 работы (несмежные не объединяются)
    int count = 0;
    while (!stack.empty()) {
        stack.pop();
        count++;
    }
    EXPECT_EQ(count, 3);
}

TEST(WorkStack, MergeFragmentedDifferentStudents) {
    WorkStack stack;
    stack += StudentWork("Ivanov", std::nullopt, 1, 2);
    stack += StudentWork("Petrov", std::nullopt, 1, 2);
    stack += StudentWork("Ivanov", std::nullopt, 3, 4);

    stack.merge_fragmented_works();

    // Ivanov должен объединиться, Petrov остаться отдельно
    int count = 0;
    while (!stack.empty()) {
        stack.pop();
        count++;
    }
    EXPECT_EQ(count, 2);
}

TEST(WorkStack, SplitAllPagesEmpty) {
    WorkStack stack;
    WorkStack result = stack.split_all_pages();
    EXPECT_TRUE(result.empty());
}

TEST(WorkStack, SplitAllPagesSinglePage) {
    WorkStack stack;
    stack += StudentWork("Ivanov", 5, 1, 1);

    WorkStack result = stack.split_all_pages();
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.pop().get_first_page(), 1);
}

TEST(WorkStack, SplitAllPagesMultiPage) {
    WorkStack stack;
    stack += StudentWork("Ivanov", 5, 1, 3);

    WorkStack result = stack.split_all_pages();

    int page_count = 0;
    while (!result.empty()) {
        StudentWork page = result.pop();
        EXPECT_EQ(page.get_first_page(), page.get_last_page());
        page_count++;
    }
    EXPECT_EQ(page_count, 3);
}

TEST(WorkStack, MultiplePushOperations) {
    WorkStack stack;

    stack += StudentWork("Ivanov", 5, 1, 3);
    stack += StudentWork("Petrov", 4, 1, 2);
    stack += StudentWork("Sidorov", std::nullopt, 1, 1);

    EXPECT_FALSE(stack.empty());

    // порядок (LIFO)
    EXPECT_EQ(stack.pop().get_surname(), "Sidorov");
    EXPECT_EQ(stack.pop().get_surname(), "Petrov");
    EXPECT_EQ(stack.pop().get_surname(), "Ivanov");
}


TEST(WorkStack, EmptyStackFill) {
    WorkStack stack;
    stack += StudentWork("Ivanov", std::nullopt, 1, 1);
    EXPECT_TRUE(stack.partially_filled());
}

TEST(WorkStack, MergeEdgeCases) {
    // Случай 1: пустой стек
    WorkStack empty;
    EXPECT_NO_THROW(empty.merge_fragmented_works());

    // Случай 2: одна работа
    WorkStack single;
    single += StudentWork("A", std::nullopt, 1, 2);
    EXPECT_NO_THROW(single.merge_fragmented_works());

    // Случай 3: несмежные работы
    WorkStack non_adjacent;
    non_adjacent += StudentWork("A", std::nullopt, 1, 2);
    non_adjacent += StudentWork("A", std::nullopt, 4, 5);
    EXPECT_NO_THROW(non_adjacent.merge_fragmented_works());
}
