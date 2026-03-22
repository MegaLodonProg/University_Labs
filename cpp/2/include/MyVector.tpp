#pragma once


// конструктор копирования
template<typename T>
MyVector<T>::MyVector(const MyVector &other)
    : size(other.size), capacity(other.capacity)
{
    data = new T[other.capacity];

    std::copy_n(other.data, other.size, this->data);
}

// конструктор перемещения
template<typename T>
MyVector<T>::MyVector(MyVector &&other) noexcept
    : data(other.data), size(other.size), capacity(other.capacity)
{
    other.data = nullptr;
    other.size = 0;
    other.capacity = 0;
}

// Для копирования данных из std::vector
template<typename T>
MyVector<T>::MyVector(const std::vector<T>& other)
        : size(static_cast<int>(other.size())), capacity(static_cast<int>(other.size()))
{
    data = new T[capacity];
    std::copy(other.begin(), other.end(), data);
}


// перегрузка оператора присваивания (копирование)
template<typename T>
MyVector<T>& MyVector<T>::operator=(const MyVector& other) noexcept{
    if (this != &other) {
        delete[] this->data;

        size = other.size;
        capacity = other.capacity;
        // можно сразу capacity, тк уже инициализировали таким же значением, что и у other
        data = new T[capacity];
        std::copy(other.data, other.data + size, this->data);
    }

    return *this;
}

// перегрузка оператора присваивания (перемещение)
template<typename T>
MyVector<T>& MyVector<T>::operator=(MyVector&& other) noexcept {
    if (this != &other) {
        delete[] this->data;

        data = other.data;
        size = other.size;
        capacity = other.capacity;

        other.data = nullptr;
        other.size = 0;
        other.capacity = 0;
    }
    return *this;
}

template<typename T>
T& MyVector<T>::operator[](const int index) {
    return data[index];
}

template<typename T>
const T& MyVector<T>::operator[](const int index) const{
    return data[index];
}

template<typename T>
T& MyVector<T>::at(const int pos) {  // Неконстантная версия
    if (pos < 0 || pos >= size) {
        throw std::out_of_range("MyVector::at");
    }
    return data[pos];
}

template<typename T>
const T& MyVector<T>::at(const int pos) const{
    if (pos < 0 || pos >= size) {
        throw std::out_of_range("MyVector::at");
    }
    return data[pos];
}

template<typename T>
T& MyVector<T>::back() {
    if (size == 0) throw std::out_of_range("MyVector::back");
    return data[size - 1];
}

template<typename T>
const T& MyVector<T>::back() const {
    if (size == 0) throw std::out_of_range("MyVector::back");
    return data[size - 1];
}

template<typename T>
T MyVector<T>::pop_back() {
    if (size == 0) throw std::out_of_range("MyVector::pop_back");
    T value = std::move(data[size - 1]);
    size--;
    return value;
}

template<typename T>
bool MyVector<T>::empty() const {
    return size == 0;
}

template<typename T>
int MyVector<T>::vec_size() const{
    return size;
}

template<typename T>
void MyVector<T>::extension(const int new_capacity) {
    if (new_capacity <= capacity){ return; }

    auto* new_data = new T[new_capacity];
    std::copy(data, data + size, new_data);

    delete[] data;
    data = new_data;
    capacity = new_capacity;
}

template<typename T>
void MyVector<T>::push_back(const T& student) {
    if (size >= capacity) {
        const int new_capacity = (capacity == 0) ? 1 : capacity * 2;
        extension(new_capacity);
    }
    data[size] = student;
    size++;
}
