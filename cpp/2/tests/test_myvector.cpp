#include <gtest/gtest.h>
#include "MyVector.hpp"
#include "StudentWork.hpp"

// IMPORTANT ======================= BASIC TYPES =======================

TEST(MyVector, IntVector) {
    MyVector<int> vec;
    vec.push_back(42);
    vec.push_back(100);

    EXPECT_EQ(vec.vec_size(), 2);
    EXPECT_EQ(vec[0], 42);
    EXPECT_EQ(vec[1], 100);
}

TEST(MyVector, DoubleVector) {
    MyVector<double> vec;
    vec.push_back(3.14);

    EXPECT_EQ(vec.vec_size(), 1);
    EXPECT_DOUBLE_EQ(vec[0], 3.14);
}

TEST(MyVector, StringVector) {
    MyVector<std::string> vec;
    vec.push_back("Hello");
    vec.push_back("World");

    EXPECT_EQ(vec.vec_size(), 2);
    EXPECT_EQ(vec.back(), "World");
}


// IMPORTANT ======================= CONSTRUCTORS AND DESTRUCTOR TESTS =======================

TEST(MyVector, DefaultConstructor) {
    const MyVector<StudentWork> vec;
    EXPECT_EQ(vec.vec_size(), 0);
    EXPECT_EQ(vec.get_capacity(), 0);
    EXPECT_TRUE(vec.empty());
}

TEST(MyVector, CopyConstructorWithZeroCapacity) {
    const MyVector<StudentWork> empty_vec;
    MyVector<StudentWork> copy(empty_vec);

    EXPECT_TRUE(copy.empty());
    EXPECT_EQ(copy.vec_size(), 0);
}

TEST(MyVector, CopyConstructor) {
    MyVector<StudentWork> original;
    original.push_back(StudentWork("Ivanov"));
    original.push_back(StudentWork("Petrov"));

    MyVector<StudentWork> copy = original; // Копирующий конструктор
    EXPECT_EQ(copy.vec_size(), 2);
    EXPECT_EQ(copy[0].get_surname(), "Ivanov");
    EXPECT_EQ(copy[1].get_surname(), "Petrov");
}

TEST(MyVector, MoveConstructor) {
    MyVector<StudentWork> original;
    original.push_back(StudentWork("Sidorov"));

    MyVector<StudentWork> moved = std::move(original); // Перемещающий конструктор
    EXPECT_EQ(moved.vec_size(), 1);
    EXPECT_EQ(moved[0].get_surname(), "Sidorov");
    EXPECT_EQ(original.vec_size(), 0);
}

// IMPORTANT ======================= ASSIGNMENT OPERATORS TESTS =======================

TEST(MyVector, CopyAssignment) {
    MyVector<StudentWork> vec1;
    vec1.push_back(StudentWork("Ivanov"));

    MyVector<StudentWork> vec2;
    vec2 = vec1; // Копирующее присваивание

    EXPECT_EQ(vec2.vec_size(), 1);
    EXPECT_EQ(vec2[0].get_surname(), "Ivanov");
}

TEST(MyVector, MoveAssignment) {
    MyVector<StudentWork> vec1;
    vec1.push_back(StudentWork("Petrov"));

    MyVector<StudentWork> vec2;
    vec2 = std::move(vec1); // Перемещающее присваивание

    EXPECT_EQ(vec2.vec_size(), 1);
    EXPECT_EQ(vec2[0].get_surname(), "Petrov");
    EXPECT_EQ(vec1.vec_size(), 0);
}

TEST(MyVector, SelfAssignment) {
    MyVector<StudentWork> vec;
    vec.push_back(StudentWork("Ivanov"));

    vec = vec; // Самоприсваивание

    EXPECT_EQ(vec.vec_size(), 1);
    EXPECT_EQ(vec[0].get_surname(), "Ivanov");
}

TEST(MyVector, ExtensionWithSmallerOrEqualCapacity) {
    MyVector<StudentWork> vec;
    vec.push_back(StudentWork("Ivanov"));

    int old_capacity = vec.get_capacity();

    EXPECT_EQ(vec.get_capacity(), old_capacity);
}

// IMPORTANT ======================= ELEMENT ACCESS TESTS =======================

TEST(MyVector, OperatorBracket) {
    MyVector<StudentWork> vec;
    vec.push_back(StudentWork("Ivanov", 5, 1, 3));

    // Чтение через operator[]
    EXPECT_EQ(vec[0].get_surname(), "Ivanov");
    EXPECT_EQ(vec[0].get_grade(), 5);

    // Запись через operator[]
    vec[0] = StudentWork("Petrov", 4, 1, 2);
    EXPECT_EQ(vec[0].get_surname(), "Petrov");
}

TEST(MyVector, OperatorBracketConst) {
    MyVector<StudentWork> vec;
    vec.push_back(StudentWork("Ivanov"));

    const MyVector<StudentWork>& const_vec = vec;
    // Должен вызваться const версия operator[]
    EXPECT_EQ(const_vec[0].get_surname(), "Ivanov");
}

TEST(MyVector, AtMethod) {
    MyVector<StudentWork> vec;
    vec.push_back(StudentWork("Ivanov"));

    EXPECT_EQ(vec.at(0).get_surname(), "Ivanov");
}

TEST(MyVector, AtMethodConst) {
    MyVector<StudentWork> vec;
    vec.push_back(StudentWork("Ivanov"));

    const MyVector<StudentWork>& const_vec = vec;
    EXPECT_EQ(const_vec.at(0).get_surname(), "Ivanov");
    EXPECT_THROW(vec.at(-1), std::out_of_range);
}

TEST(MyVector, ConstAtThrowsOnInvalidIndex) {
    const MyVector<int> vec = std::vector<int>({3, 4, 5});

    EXPECT_THROW(vec.at(-1), std::out_of_range);
    int return_value = vec.at(0);
    EXPECT_NO_THROW(return_value = vec.at(0));
    EXPECT_EQ(return_value, 3);

    EXPECT_THROW(vec.at(3), std::out_of_range);
    EXPECT_THROW(vec.at(10), std::out_of_range);
}

TEST(MyVector, AtMethodOutOfRange) {
    MyVector<StudentWork> vec;
    EXPECT_THROW(vec.at(3), std::out_of_range); // Пустой вектор
}

TEST(MyVector, BackMethod) {
    MyVector<StudentWork> vec;
    vec.push_back(StudentWork("First"));
    vec.push_back(StudentWork("Last"));

    EXPECT_EQ(vec.back().get_surname(), "Last");
}

TEST(MyVector, BackMethodConst) {
    MyVector<StudentWork> vec;
    vec.push_back(StudentWork("Last"));

    const MyVector<StudentWork>& const_vec = vec;
    EXPECT_EQ(const_vec.back().get_surname(), "Last");
}

TEST(MyVector, BackMethodEmpty) {
    MyVector<StudentWork> vec;
    EXPECT_THROW(vec.back(), std::out_of_range);
}

// IMPORTANT ======================= MODIFIERS TESTS =======================
TEST(MyVector, PushBack) {
    MyVector<StudentWork> vec;

    vec.push_back(StudentWork("Ivanov"));
    EXPECT_EQ(vec.vec_size(), 1);
    EXPECT_EQ(vec[0].get_surname(), "Ivanov");

    const StudentWork petrov = {"Petrov", 4, 1, 2};
    vec.push_back(petrov);
    EXPECT_EQ(vec.vec_size(), 2);
    EXPECT_EQ(vec[1].get_surname(), "Petrov");
}

TEST(MyVector, PushBackCapacityExtension) {
    MyVector<StudentWork> vec;

    // Добавляем много элементов для проверки расширения capacity
    for (int i = 0; i < 10; ++i) {
        vec.push_back(StudentWork("Student" + std::to_string(i)));
    }

    EXPECT_EQ(vec.vec_size(), 10);
    EXPECT_GE(vec.get_capacity(), vec.vec_size()); // capacity должен быть >= size
}

TEST(MyVector, PopBack) {
    MyVector<StudentWork> vec;
    vec.push_back(StudentWork("Ivanov"));
    vec.push_back(StudentWork("Petrov"));

    StudentWork return_work = vec.pop_back();
    std::cout << return_work.get_surname() << std::endl;
    EXPECT_EQ(return_work.get_surname(), "Petrov");

    EXPECT_EQ(vec.vec_size(), 1);
    EXPECT_EQ(vec[0].get_surname(), "Ivanov");
    vec.pop_back();
    EXPECT_EQ(vec.vec_size(), 0);
    EXPECT_TRUE(vec.empty());
}

TEST(MyVector, PopBackEmpty) {
    MyVector<StudentWork> vec;
    EXPECT_THROW(vec.pop_back(), std::out_of_range);
    EXPECT_TRUE(vec.empty());
}

TEST(MyVector, ConstBackMethodEmpty) {
    const MyVector<StudentWork> vec;
    EXPECT_THROW(vec.back(), std::out_of_range);
}

// IMPORTANT ======================= EDGE CASES TESTS =======================

TEST(MyVector, LargeNumberOfElements) {
    MyVector<StudentWork> vec;

    // Проверка работы с большим количеством элементов
    const int count = 100;
    for (int i = 0; i < count; ++i) {
        vec.push_back(StudentWork("Student" + std::to_string(i)));
    }

    EXPECT_EQ(vec.vec_size(), count);

    // Проверяем, что все элементы на месте
    for (int i = 0; i < count; ++i) {
        EXPECT_EQ(vec[i].get_surname(), "Student" + std::to_string(i));
    }
}

TEST(MyVector, CopyAfterModifications) {
    MyVector<StudentWork> original;
    original.push_back(StudentWork("Ivanov"));
    original.push_back(StudentWork("Petrov"));

    MyVector<StudentWork> copy = original;

    // Модифицируем оригинал - копия не измениться
    original[0] = StudentWork("Sidorov");
    original.pop_back();

    EXPECT_EQ(copy.vec_size(), 2);
    EXPECT_EQ(copy[0].get_surname(), "Ivanov"); // Должен остаться старым
    EXPECT_EQ(copy[1].get_surname(), "Petrov");
}

TEST(MyVector, MoveThenUseOriginal) {
    MyVector<StudentWork> original;
    original.push_back(StudentWork("Ivanov"));

    MyVector<StudentWork> moved = std::move(original);

    // original теперь пустой, но валидный
    EXPECT_NO_THROW(original.vec_size());
    EXPECT_NO_THROW(original.empty());
    EXPECT_THROW(original.back(), std::out_of_range);
}