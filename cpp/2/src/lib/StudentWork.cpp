#include <iostream>
#include <utility>
#include <vector>
#include "StudentWork.hpp"

StudentWork::StudentWork(std::string surname, const std::optional<int> grade, const int first_page, const int last_page)
        : surname(std::move(surname)), grade(grade), first_page(first_page), last_page(last_page)
{
    if (grade.has_value()) {
        if (first_page != 1) {
            throw std::invalid_argument("The grade can only be present on the first page");
        }
        if (!is_valid_grade(grade.value())) {
            throw std::invalid_argument("Grade must be between 2 and 5");
        }
    }
    if (!is_valid_pages(first_page, last_page)) {
        throw std::invalid_argument("Invalid page numbers");
    }
};

void StudentWork::set_grade(const std::optional<int> new_g) {
    if (first_page != 1 && new_g != std::nullopt) {
        throw std::invalid_argument("The grade can only be present on the first page");
    }
    if (new_g.has_value() && !is_valid_grade(new_g.value())) {
        throw std::invalid_argument("Grade must be between 2 and 5");
    }
    grade = new_g;
}

void StudentWork::set_pages(const int new_first, const int new_last) {
    if (!is_valid_pages(new_first, new_last)) {
        throw std::invalid_argument("Invalid page numbers");
    }
    first_page = new_first;
    last_page = new_last;
}

void StudentWork::set_surname(const std::string& new_sur) {
    surname = new_sur;
}

int StudentWork::compare(const StudentWork &other) const {
    int surname_cmp = surname.compare(other.surname);
    if (surname_cmp != 0) {
        return surname_cmp;
    }

    if (this->first_page < other.first_page) return -1;
    if (this->first_page > other.first_page) return 1;
    return 0;
}


std::vector<StudentWork> StudentWork::split_into_pages() const {
    std::vector<StudentWork> split_work;
    split_work.reserve(last_page - first_page + 1); // добавление оптимизации
    for (int i = first_page; i <= last_page; i++) {
        const auto grade_for_page = (i == 1) ? (grade) : std::nullopt;
        StudentWork list_of_work {surname, grade_for_page, i, i};
        split_work.push_back(list_of_work);
    }
    return split_work;
}

StudentWork StudentWork::operator+(const StudentWork &other) const {
    const bool is_one_person = surname == other.surname;
    const bool is_nearby_parts = (last_page + 1 == other.first_page) || (other.last_page + 1 == first_page);
    if (is_one_person && is_nearby_parts) {
        const int min_page = std::min(first_page, other.first_page);
        const int max_page = std::max(last_page, other.last_page);
        std::optional<int> work_grade = std::nullopt;
        if (grade.has_value()) work_grade = grade.value();
        else if (other.grade.has_value()) work_grade = other.grade.value();

        StudentWork connected_work {surname, work_grade, min_page, max_page};
        return connected_work;
    }
    throw std::logic_error("Only the nearby parts of one person's work can be connected");
}

void StudentWork::input(std::istream& is) {
    std::string temp_surname;
    int temp_grade;
    int temp_first, temp_last;

    if (!(is >> temp_surname >> temp_grade >> temp_first >> temp_last)) {
        throw std::runtime_error("Error parsing student work data");
    }

    set_surname(temp_surname);
    set_pages(temp_first, temp_last);
    set_grade((temp_grade == -1) ? std::nullopt : std::optional<int>(temp_grade));
}

void StudentWork::output(std::ostream& os) const {
    os << "Surname: " << surname << "\n";
    os << "Grade: ";
    if (grade.has_value()) {
        os << grade.value();
    } else {
        os << "not stated";
    }
    os << "\nPages: " << first_page << " - " << last_page << "\n";
}


std::istream& operator>>(std::istream& is, StudentWork& work) {
    work.input(is);
    return is;
}

std::ostream& operator<<(std::ostream& os, const StudentWork& work) {
    work.output(os);
    return os;
}