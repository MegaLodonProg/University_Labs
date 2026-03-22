#include <gtest/gtest.h>
#include <sstream>
#include "StudentWork.hpp"

// IMPORTANT : instructions for use GTests
/* TEST(имя тестируемого класса, уникальное имя конкретного теста) - макрос, который объявляет тестовый случай.
 * Структура теста - Arrange-Act-Assert
 * Arrange (Подготовка) : StudentWork work("Ivanov"); // Подготовили тестовый объект
 * Act (Действие) : work.set_grade(5); // Вызываем метод, который тестируем
 * Assert (Проверка) : EXPECT_EQ(work.get_grade().value(), 5); // Проверка, что действие привело к ожидаемому результату
 * Important : Макросы проверок (Assertions)
 * EXPECT_EQ(expected, actual) - Проверяет, что два значения равны. EXPECT_EQ(work.get_surname(), "Ivanov");
 * EXPECT_FALSE(condition) - Проверяет, что условие ложно. EXPECT_FALSE(work.get_grade().has_value());
 * EXPECT_TRUE(cond) - Проверяет, что условие истинно. EXPECT_TRUE(work.is_valid())
 * EXPECT_NE(a, b) - Проверяет, что значения не равны. EXPECT_NE(work.get_grade(), 2)
 * EXPECT_THROW(statement, exception) - Проверяет, что Что выражение бросает исключение. EXPECT_THROW(StudentWork("", 6), std::invalid_argument)
 * EXPECT_NO_THROW(statement) - Проверяет, что выражение не бросает исключение. EXPECT_NO_THROW(StudentWork("Ivanov"))
 */


// IMPORTANT ======================= CORRECT CONSTRUCTIONS TESTS =======================

// Проверка конструктора с инициализацией только фамилии
TEST(StudentWork, CreationWithSurnameOnly) {
    StudentWork work("Ivanov");

    EXPECT_EQ(work.get_surname(), "Ivanov");
    EXPECT_FALSE(work.get_grade().has_value());
    EXPECT_EQ(work.get_first_page(), 1);
    EXPECT_EQ(work.get_last_page(), 1);
}

// Проверка конструктора с инициализацией всех параметров
TEST(StudentWork, CreationWithAllParameters) {
    StudentWork work("Ivanov", 4, 1, 12);

    EXPECT_EQ(work.get_surname(), "Ivanov");
    EXPECT_TRUE(work.get_grade().has_value());
    EXPECT_EQ(work.get_grade().value(), 4);
    EXPECT_EQ(work.get_first_page(), 1);
    EXPECT_EQ(work.get_last_page(), 12);
}

// IMPORTANT ======================= WRONG CONSTRUCTIONS TESTS =======================

TEST(StudentWork, CreationWithGradeNotOnFirstPage) {
    EXPECT_THROW(
        StudentWork work("Ivanov", 4, 2, 12),
        std::invalid_argument
    );
}

TEST(StudentWorkTest, CreationWithInvalidGrade_ThrowsException) {
    EXPECT_THROW(
        StudentWork("Ivanov", 6, 1, 12),
        std::invalid_argument
    );

    EXPECT_THROW(
        StudentWork("Ivanov", 1, 1, 12),
        std::invalid_argument
    );
}

TEST(StudentWorkTest, CreationWithInvalidPages_ThrowsException) {
    EXPECT_THROW(
        StudentWork("Ivanov", std::nullopt, 5, 3),
        std::invalid_argument
    );

    EXPECT_THROW(
        StudentWork("Ivanov", std::nullopt, 0, 5),
        std::invalid_argument
    );
}

// IMPORTANT ======================= SETTER TESTS =======================

TEST(StudentWorkTest, SetNewGrade) {
    StudentWork work {"Ivanov", std::nullopt, 1, 7};
    work.set_grade(4);
    EXPECT_EQ(work.get_grade(), 4);
}

TEST(StudentWorkTest, SetNewGradeNotOnFirtsPage) {
    StudentWork work {"Ivanov", std::nullopt, 6, 7};
    EXPECT_THROW(work.set_grade(4), std::invalid_argument);
}

TEST(StudentWorkTest, SetNewGradeNotOnDiapazone) {
    StudentWork work {"Ivanov", 3, 1, 7};
    EXPECT_THROW(work.set_grade(7), std::invalid_argument);
}

TEST(StudentWorkTest, SetNewSurname) {
    StudentWork work {"Ivanov", 3, 1, 7};
    work.set_surname("Pavlov");
    EXPECT_EQ(work.get_surname(), "Pavlov");
}

TEST(StudentWorkTest, SetNewPagesOnDiapazone) {
    StudentWork work {"Ivanov", 3, 1, 7};
    work.set_pages(1, 13);
    EXPECT_EQ(work.get_first_page(), 1);
    EXPECT_EQ(work.get_last_page(), 13);
}

TEST(StudentWorkTest, SetNewIvalidPages) {
    StudentWork work {"Ivanov", 3, 1, 7};
    EXPECT_THROW(work.set_pages(-1, 5), std::invalid_argument);
}

TEST(StudentWorkTest, SetInvalidPagesEdgeCase) {
    StudentWork work("Ivanov", std::nullopt, 1, 5);
    EXPECT_THROW(work.set_pages(3, 2), std::invalid_argument);
}


// IMPORTANT ======================= COMPARISON OPERATORS TESTS =======================

TEST(StudentWorkTest, CompareSameObject) {
    StudentWork work("Ivanov", 5, 1, 10);
    EXPECT_EQ(work.compare(work), 0);
    EXPECT_TRUE(work == work);
}

TEST(StudentWorkTest, CompareDifferentSurnames) {
    StudentWork ivanov("Ivanov", std::nullopt, 1, 5);
    StudentWork petrov("Petrov", std::nullopt, 1, 5);

    // EXPECT_LT(val1, val2) проверяет, что val1 < val2
    EXPECT_LT(ivanov.compare(petrov), 0);
    EXPECT_TRUE(ivanov < petrov);
    EXPECT_TRUE(ivanov != petrov);
}

TEST(StudentWorkTest, CompareSameSurnameDifferentFirstPage) {
    StudentWork work1("Ivanov", 5, 1, 5);
    StudentWork work2("Ivanov", std::nullopt, 6, 10);


    EXPECT_LT(work1.compare(work2), 0);
    EXPECT_TRUE(work1 < work2);
    EXPECT_TRUE(work2 > work1);
}

// IMPORTANT ======================= OPERATOR + (CONCATENATION) TESTS =======================

TEST(StudentWorkTest, OperatorPlusSuccessfulConcatenation) {
    StudentWork part1("Ivanov", 5, 1, 5);
    StudentWork part2("Ivanov", std::nullopt, 6, 10);

    StudentWork combined = part1 + part2;

    EXPECT_EQ(combined.get_surname(), "Ivanov");
    EXPECT_EQ(combined.get_grade(), 5);
    EXPECT_EQ(combined.get_first_page(), 1);
    EXPECT_EQ(combined.get_last_page(), 10);
}

TEST(StudentWorkTest, OperatorPlusThrowsOnDifferentSurnames) {
    StudentWork ivanov("Ivanov", 5, 1, 5);
    StudentWork petrov("Petrov", 4, 1, 10);

    EXPECT_THROW(ivanov + petrov, std::logic_error);
}

TEST(StudentWorkTest, OperatorPlusThrowsOnNonAdjacentParts) {
    StudentWork part1("Ivanov", 5, 1, 5);
    StudentWork part2("Ivanov", std::nullopt, 7, 10);

    EXPECT_THROW(part1 + part2, std::logic_error);
}

TEST(StudentWorkTest, OperatorPlusBothGradesNullopt) {
    StudentWork part1("Ivanov", std::nullopt, 1, 3);
    StudentWork part2("Ivanov", std::nullopt, 4, 6);

    StudentWork combined = part1 + part2;

    EXPECT_EQ(combined.get_surname(), "Ivanov");
    EXPECT_FALSE(combined.get_grade().has_value());  // Оба grade = nullopt
    EXPECT_EQ(combined.get_first_page(), 1);
    EXPECT_EQ(combined.get_last_page(), 6);
}

TEST(StudentWorkTest, OperatorPlusThrowsOnSamePersonButNotNearby) {
    StudentWork part1("Ivanov", std::nullopt, 1, 3);
    StudentWork part2("Ivanov", std::nullopt, 5, 7);  // Пропуск страницы 4

    EXPECT_THROW(part1 + part2, std::logic_error);
}

// IMPORTANT ======================= SPLIT_INTO_PAGES TESTS =======================

TEST(StudentWorkTest, SplitIntoPagesMultiPageWork) {
    StudentWork original("Ivanov", std::nullopt, 3, 5);

    std::vector<StudentWork> pages = original.split_into_pages();

    EXPECT_EQ(pages.size(), 3);

    EXPECT_EQ(pages[0].get_surname(), "Ivanov");
    EXPECT_EQ(pages[0].get_first_page(), 3);
    EXPECT_EQ(pages[0].get_last_page(), 3);
    EXPECT_FALSE(pages[0].get_grade().has_value());

    EXPECT_EQ(pages[1].get_surname(), "Ivanov");
    EXPECT_EQ(pages[1].get_first_page(), 4);
    EXPECT_EQ(pages[1].get_last_page(), 4);
    EXPECT_FALSE(pages[1].get_grade().has_value());

    EXPECT_EQ(pages[2].get_surname(), "Ivanov");
    EXPECT_EQ(pages[2].get_first_page(), 5);
    EXPECT_EQ(pages[2].get_last_page(), 5);
    EXPECT_FALSE(pages[2].get_grade().has_value());
}

TEST(StudentWorkTest, SplitIntoPagesSinglePageWorkWithGrade) {
    StudentWork original("Ivanov", 5, 1, 1);

    std::vector<StudentWork> pages = original.split_into_pages();

    ASSERT_EQ(pages.size(), 1);
    EXPECT_EQ(pages[0].get_surname(), "Ivanov");
    EXPECT_EQ(pages[0].get_first_page(), 1);
    EXPECT_EQ(pages[0].get_last_page(), 1);
    EXPECT_EQ(pages[0].get_grade(), 5);
}

TEST(StudentWorkTest, SplitIntoPagesWithGradeOnNonFirstPage) {
    StudentWork work("Ivanov", 5, 1, 3);
    auto pages = work.split_into_pages();

    EXPECT_FALSE(pages[1].get_grade().has_value());
    EXPECT_FALSE(pages[2].get_grade().has_value());
}

TEST(StudentWorkTest, SplitIntoPagesWithLargeRange) {
    StudentWork work("Ivanov", std::nullopt, 1, 1000);
    EXPECT_NO_THROW(auto pages = work.split_into_pages());
}

// IMPORTANT ======================= STREAM OPERATORS TESTS =======================

TEST(StudentWorkIO, BasicInputOutput) {
    StudentWork work;
    std::stringstream ss("Ivanov 5 1 10");

    ASSERT_NO_THROW(ss >> work);
    EXPECT_EQ(work.get_surname(), "Ivanov");
    EXPECT_EQ(work.get_grade().value(), 5);
    EXPECT_EQ(work.get_first_page(), 1);
    EXPECT_EQ(work.get_last_page(), 10);

    std::stringstream output;
    output << work;
    std::string result = output.str();

    EXPECT_NE(result.find("Ivanov"), std::string::npos);
    EXPECT_NE(result.find("5"), std::string::npos);
    EXPECT_NE(result.find("1 - 10"), std::string::npos);
}

TEST(StudentWorkIO, InputWithoutGrade) {
    StudentWork work;
    std::stringstream ss("Petrov -1 1 8");

    ASSERT_NO_THROW(ss >> work);
    EXPECT_EQ(work.get_surname(), "Petrov");
    EXPECT_FALSE(work.get_grade().has_value()); // Оценка не установлена
    EXPECT_EQ(work.get_first_page(), 1);
    EXPECT_EQ(work.get_last_page(), 8);
}

TEST(StudentWorkIO, InputThrowsOnInvalidData) {
    StudentWork work;
    std::stringstream ss("Smith 6 1 5");

    EXPECT_THROW(ss >> work, std::invalid_argument);
}

TEST(StudentWorkIO, OperatorChaining) {
    std::stringstream ss("Sidorov 4 2 7\nKuznetsov -1 5 9");
    StudentWork work1, work2;

    EXPECT_THROW(ss >> work1, std::invalid_argument);

    ss >> work2;

    EXPECT_EQ(work1.get_surname(), "Sidorov");
    EXPECT_EQ(work2.get_surname(), "Kuznetsov");
    EXPECT_FALSE(work2.get_grade().has_value());
}

TEST(StudentWorkTest, InputThrowsOnParseError) {
    StudentWork work;
    std::stringstream ss("Invalid Data Here");

    EXPECT_THROW(ss >> work, std::runtime_error);
}

TEST(StudentWorkTest, InputOutputComprehensive) {
    StudentWork work1, work2;
    std::stringstream ss("Petrov 4 1 10\nSidorov -1 2 8");

    // Первый объект - с оценкой
    EXPECT_NO_THROW(ss >> work1);
    EXPECT_EQ(work1.get_surname(), "Petrov");
    EXPECT_EQ(work1.get_grade(), 4);

    // Второй объект - без оценки
    EXPECT_NO_THROW(ss >> work2);
    EXPECT_EQ(work2.get_surname(), "Sidorov");
    EXPECT_FALSE(work2.get_grade().has_value());

    // Проверим вывод
    std::stringstream output;
    output << work1;
    std::string result = output.str();
    EXPECT_NE(result.find("Petrov"), std::string::npos);
    EXPECT_NE(result.find("4"), std::string::npos);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}