#include "WorkStack.hpp"

WorkStack::WorkStack(const MyVector<StudentWork>& works) {
    for (int i = 0; i < works.vec_size(); ++i) {
        this->stack.push_back(works[i]);
    }
}

WorkStack::WorkStack(const StudentWork* works, const size_t count) {
    for (size_t i = 0; i < count; ++i) {
        stack.push_back(works[i]);
    }
}

StudentWork WorkStack::pop() {
    if (stack.empty()) {
        throw std::out_of_range("WorkStack::pop(): empty stack");
    }
    StudentWork work = this->stack.pop_back();
    return work;
}

StudentWork WorkStack::pop_without_grade() {
    for (int i = stack.vec_size() - 1; i >= 0; --i) {
        if (stack[i].get_grade() == std::nullopt) {
            StudentWork work = stack[i];
            for (int j = i; j < stack.vec_size() - 1; j++) {
                stack[j] = stack[j + 1];
            }
            stack.pop_back();
            return work;
        }
    }
    throw std::runtime_error("WorkStack::pop_without_grade(): work without grade not found");
}

bool WorkStack::empty() const {
    return stack.empty();
}

bool WorkStack::partially_filled() const { return !stack.empty(); }

WorkStack& WorkStack::operator=(const WorkStack& other) noexcept {
    if (this != &other) {
        this->stack = other.stack;
    }
    return *this;
}

WorkStack& WorkStack::operator+=(const StudentWork& work) {
    stack.push_back(work);
    return *this;
}

WorkStack WorkStack::split_all_pages() const{
    WorkStack stack_split_works;
    for (int i = 0; i < stack.vec_size(); ++i) {
        MyVector<StudentWork> split_work = stack[i].split_into_pages();
        for (int j = 0; j < split_work.vec_size(); ++j) {
            stack_split_works += split_work[j];
        }
    }
    return stack_split_works;
}

void WorkStack::sort_works(MyVector<StudentWork>& works) {
    for (int i = 0; i < works.vec_size(); i++) {
        for (int j = i + 1; j < works.vec_size(); j++) {
            if (works[j] < works[i]) {
                std::swap(works[i], works[j]);
            }
        }
    }
}

void WorkStack::merge_fragmented_works() {
    if (stack.empty()) return;

    sort_works(stack);

    MyVector<StudentWork> merged;

    while (!stack.empty()) {
        StudentWork merge_work = stack.pop_back();
        while (!stack.empty()) {
            StudentWork last_work = stack.back();
            try {
                merge_work = last_work + merge_work;
                stack.pop_back();
            }
            catch (const std::logic_error& e) {
                break;
            }
        }
        merged.push_back(merge_work);
    }
    stack = merged;
}


std::ostream& operator<<(std::ostream& os, const WorkStack& works) {
    for (int i = 0; i < works.stack.vec_size(); i++) {
        os << works.stack[i];
        if (i < works.stack.vec_size() - 1) {
            os << "\n";
        }
    }
    return os;
}

std::istream& operator>>(std::istream& is, WorkStack& works) {
    works.stack = MyVector<StudentWork>();

    StudentWork work;
    while (is >> work) {
        works.stack.push_back(work);

        // Проверяет следующий символ, если конец файла или неверные данные - выходим
        if (is.peek() == EOF || !is.good()) {
            break;
        }
    }
    return is;
}




