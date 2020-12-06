#pragma once

#include <iterator>
#include <cassert>
#include <initializer_list>
#include <cstring>
#include <stdexcept>
#include <exception>
#include <algorithm>
#include <utility>


class ReserveProxyObj {
public:
    explicit ReserveProxyObj(size_t size) {
        this->size_ = size;
    }
    size_t size_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    explicit SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) {
        this->capacity_ = size;
        this->size_ = size;
        this->items_ = new Type[size]();
        for (size_t ptr = 0; ptr < this->capacity_; ptr++) {
            this->items_[ptr] = 0;
        }
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) {
        this->capacity_ = size;
        this->size_ = size;
        this->items_ = new Type[this->size_]();
        for (size_t ptr = 0; ptr < this->size_; ptr++) {
            this->items_[ptr] = value;
        }
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        SimpleVector tmp_vec(init.size());
        std::copy(init.begin(), init.end(), tmp_vec.begin());
        this->swap(tmp_vec);
    }

    SimpleVector(const SimpleVector& other) {
        if (this != &other) {
            this->size_ = other.size_;
            this->capacity_ = other.capacity_;
            this->items_ = new Type[other.capacity_];
            std::copy(other.begin(), other.end(), begin());
        }
    }

    SimpleVector(SimpleVector&& other) {
        std::swap(other.items_, this->items_);
        std::swap(other.size_, this->size_);
        std::swap(other.capacity_, this->capacity_);
        other.items_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    SimpleVector(ReserveProxyObj obj_) {
        this->capacity_ = obj_.size_;
        this->size_ = 0;
        this->items_ = new Type[obj_.size_]();
        for (size_t ptr = 0; ptr < this->capacity_; ptr++) {
            this->items_[ptr] = 0;
        }
    }

    ~SimpleVector() {
        delete[] this->items_;
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (rhs.size_ < this->capacity_) {
            std::copy(rhs.begin(), rhs.end(), this->begin());
            this->size_ = rhs.size_;
        }
        else {
            SimpleVector tmp(std::move(rhs));
            std::swap(tmp.items_, this->items_);
            std::swap(tmp.size_, this->size_);
            std::swap(tmp.capacity_, this->capacity_);
        }
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& rhs) {
        SimpleVector tmp(std::move(rhs));
        std::swap(tmp.items_, this->items_);
        std::swap(tmp.size_, this->size_);
        std::swap(tmp.capacity_, this->capacity_);
        return *this;
    }

    void swap(SimpleVector& other) noexcept {
        std::swap(this->size_, other.size_);
        std::swap(this->capacity_, other.capacity_);
        std::swap(this->items_, other.items_);
    }

    void PushBack(const Type& item) {
        this->Resize(this->size_ + 1);
        this->items_[this->size_ - 1] = item;
    }

    void PushBack(Type&& item) {
        this->Resize(this->size_ + 1);
        this->items_[this->size_ - 1] = std::move(item);
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        if (this->size_ < this->capacity_) {
            Iterator result = this->begin() + (pos - this->cbegin());
            for (Iterator iter = this->end(); iter != pos; --iter) {
                *iter = *(iter - 1);
            }
            *result = value;
            ++this->size_;
            return result;
        }
        else {
            this->capacity_ == 0 ? this->capacity_ = 1 : this->capacity_ += this->capacity_;
            Type* new_arr_tmp = new Type[this->capacity_]();
            std::copy(this->cbegin(), pos, new_arr_tmp);
            size_t position = pos - this->cbegin(); //SUQQQOO!
            std::fill(new_arr_tmp + position, new_arr_tmp + position + 1, value); //SUQQQOO!
            std::copy(pos, this->cend(), new_arr_tmp + position + 1);
            std::swap(new_arr_tmp, this->items_);
            delete[] new_arr_tmp;
            this->size_++;
            return (this->begin() + position); //SUQQQOO!
        }
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        int result = pos - this->begin();
        if (this->capacity_ != this->size_) {
            std::move_backward(this->begin() + result, this->end(), this->end() + 1);
            *(this->begin() + result) = std::move(value);
        }
        else {
            Type* new_arr_tmp = new Type[std::max((size_t)1, this->size_ * 2)]();
            std::move(this->begin(), this->begin() + result, new_arr_tmp);
            new_arr_tmp[result] = std::move(value);
            std::move(this->begin() + result, this->end(), new_arr_tmp + result + 1);
            delete[] this->items_;
            this->items_ = new_arr_tmp;
            this->capacity_ = std::max((size_t)1, this->size_ * 2);
        }
        this->size_++;
        return (this->begin() + result);
    }

    void PopBack() noexcept {
        this->size_--;
    }

    Iterator Erase(ConstIterator pos) {
        Iterator pos_it = const_cast<Iterator>(pos);
        std::move(pos_it + 1, this->end(), pos_it);
        this->size_--;
        return pos_it;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > this->capacity_) {
            Type* new_arr_tmp = new Type[new_capacity]();
            std::copy(this->cbegin(), this->cend(), new_arr_tmp);
            delete[] this->items_;
            this->items_ = new_arr_tmp;
            this->capacity_ = new_capacity;
        }
    }

    // Возвращает количество элементов в массиве
    [[nodiscard]] size_t GetSize() const noexcept {
        return this->size_;
    }

    // Возвращает вместимость массива
    [[nodiscard]] size_t GetCapacity() const noexcept {
        return this->capacity_;
    }

    // Сообщает, пустой ли массив
    [[nodiscard]] bool IsEmpty() const noexcept {
        return !this->size_;
    }

    // Возвращает ссылку на элемент с индексом index
    [[nodiscard]] Type& operator[](size_t index) noexcept {
        return this->items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    [[nodiscard]] const Type& operator[](size_t index) const noexcept {
        return this->items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= this->size_) {
            throw std::out_of_range("Argument to big");
        }
        else {
            return this->items_[index];
        }
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= this->size_) {
            throw std::out_of_range("Argument to big");
        }
        else {
            return this->items_[index];
        }
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        std::fill(this->begin(), this->end(), 0);
        this->size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size > capacity_) {
            Type* new_data = new Type[std::max(new_size, this->size_ * 2)];
            std::move(this->begin(), this->end(), new_data);
            std::swap(new_data, this->items_);
            delete[] new_data;
            this->capacity_ = std::max(new_size, this->size_ * 2);
            for (size_t ptr = this->size_; ptr < this->capacity_; ptr++) {
                this->items_[ptr] = std::move(0);
            }
        }
        else if (new_size > this->size_) {
            for (auto first = this->items_ + this->size_, last = this->items_ + new_size; first != last; ++first) {
                *first = Type{};
            }
        }
        this->size_ = new_size;
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return this->items_;
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return (this->begin() + this->GetSize());
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return this->items_;
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return (this->begin() + this->GetSize());
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return this->items_;
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return (this->begin() + this->GetSize());
    }

private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    Type* items_ = nullptr;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    size_t ptr_lhs = 0;
    size_t ptr_rhs = 0;
    for (; ptr_lhs < lhs.GetSize(); ptr_lhs++, ptr_rhs++) {
        if (!(lhs[ptr_lhs] == rhs[ptr_rhs])) {
            return false;
        }
    }
    return true;
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    auto ptr_lhs = lhs.begin();
    auto ptr_rhs = rhs.begin();
    for (; (ptr_lhs != lhs.begin() + lhs.GetSize()) && (ptr_rhs != rhs.begin() + rhs.GetSize()); ptr_lhs++, ptr_rhs++) {
        if (*ptr_lhs < *ptr_rhs) {
            return true;
        }
        if (*ptr_rhs < *ptr_lhs) {
            return false;
        }
    }
    return (ptr_lhs == lhs.begin() + lhs.GetSize()) && (ptr_rhs != rhs.begin() + rhs.GetSize());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}



