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

template <typename TType>
class SimpleVector {
public:
    using Iterator = TType*;
    using ConstIterator = const TType*;

    explicit SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size) {
        this->capacity_ = size;
        this->size_ = size;
        this->items_ = new TType[this->capacity_]();
        std::fill(begin(), end(), TType{0});
    }

    SimpleVector(size_t size, const TType& value) {
        this->capacity_ = size;
        this->size_ = size;
        this->items_ = new TType[this->capacity_]();
        std::fill(begin(), end(), value);
    }

    SimpleVector(std::initializer_list<TType> init) {
        SimpleVector tmp_vec(init.size());
        std::copy(init.begin(), init.end(), tmp_vec.begin());
        this->swap(tmp_vec);
    }

    SimpleVector(const SimpleVector& other) {
        if (this != &other) {
            this->size_ = other.size_;
            this->capacity_ = other.capacity_;
            this->items_ = new TType[other.capacity_]();
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
        this->items_ = new TType[obj_.size_]();
        std::fill(begin(), end(), TType{0});
    }

    ~SimpleVector() {
        delete[] this->items_;
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if(&rhs != this) {
            if (rhs.size_ < this->capacity_) {
                std::copy(rhs.begin(), rhs.end(), this->begin());
                this->size_ = rhs.size_;
            } else {
                SimpleVector tmp(std::move(rhs));
                std::swap(tmp.items_, this->items_);
                std::swap(tmp.size_, this->size_);
                std::swap(tmp.capacity_, this->capacity_);
            }
        }
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& rhs) {
        if (&rhs != this) {
            SimpleVector tmp(std::move(rhs));
            std::swap(tmp.items_, this->items_);
            std::swap(tmp.size_, this->size_);
            std::swap(tmp.capacity_, this->capacity_);
        }
        return *this;
    }

    //I remember that i need to name my method like Swap, but its in precode, so i dicede to 
    //not change the name because joi need to pass test system cheks(in other way i named it Swap))))
    void swap(SimpleVector& other) noexcept {
        std::swap(this->size_, other.size_);
        std::swap(this->capacity_, other.capacity_);
        std::swap(this->items_, other.items_);
    }

    void PushBack(const TType& item) {
        auto cpy_item = item;
        this->PushBack(std::move(cpy_item));
    }

    void PushBack(TType&& item) {
        this->Resize(this->size_ + 1);
        this->items_[this->size_ - 1] = std::move(item);
    }

    Iterator Insert(ConstIterator pos, const TType& value) {
        auto cpy_item = value;
        this->Insert(pos, std::move(cpy_item));
    }

    Iterator Insert(ConstIterator pos, TType&& value) {
        int result = pos - this->begin();
        if (this->capacity_ != this->size_) {
            std::move_backward(this->begin() + result, this->end(), this->end() + 1);
            *(this->begin() + result) = std::move(value);
        } else {
            TType* new_arr_tmp = new TType[std::max((size_t)1, this->size_ * 2)]();
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
            TType* new_arr_tmp = new TType[new_capacity]();
            std::copy(this->cbegin(), this->cend(), new_arr_tmp);
            delete[] this->items_;
            this->items_ = new_arr_tmp;
            this->capacity_ = new_capacity;
        }
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return this->size_;
    }

    [[nodiscard]] size_t GetCapacity() const noexcept {
        return this->capacity_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return !this->size_;
    }

    [[nodiscard]] TType& operator[](size_t index) noexcept {
        return this->items_[index];
    }

    [[nodiscard]] const TType& operator[](size_t index) const noexcept {
        return this->items_[index];
    }

    TType& At(size_t index) {
        if (index >= this->size_) {
            throw std::out_of_range("Argument to big");
        } else {
            return this->items_[index];
        }
    }

    const TType& At(size_t index) const {
        if (index >= this->size_) {
            throw std::out_of_range("Argument to big");
        } else {
            return this->items_[index];
        }
    }

    void Clear() noexcept {
        std::fill(this->begin(), this->end(), TType{0});
        this->size_ = 0;
    }

    void Resize(size_t new_size) {
        if (new_size > capacity_) {
            this->capacity_ = std::max(new_size, this->capacity_ * 2);
            TType* new_data = new TType[this->capacity_]();
            std::move(this->begin(), this->end(), new_data);
            std::swap(new_data, this->items_);
            delete[] new_data;
            for (size_t ptr = this->size_; ptr < this->capacity_; ptr++) {
                this->items_[ptr] = std::move(TType{0});
            }
        } else if (new_size > this->size_) {
            for (auto first = this->items_ + this->size_, last = this->items_ + new_size; first != last; ++first) {
                *first = TType{0};
            }
        }
        this->size_ = new_size;
    }

    Iterator begin() noexcept {
        return this->items_;
    }

    Iterator end() noexcept {
        return (this->begin() + this->GetSize());
    }

    ConstIterator begin() const noexcept {
        return this->items_;
    }

    ConstIterator end() const noexcept {
        return (this->begin() + this->GetSize());
    }

    ConstIterator cbegin() const noexcept {
        return this->items_;
    }

    ConstIterator cend() const noexcept {
        return (this->begin() + this->GetSize());
    }

private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    TType* items_ = nullptr;
};

template <typename TType>
inline bool operator==(const SimpleVector<TType>& lhs, const SimpleVector<TType>& rhs) {
    size_t ptr_lhs = 0;
    size_t ptr_rhs = 0;
    for (; ptr_lhs < lhs.GetSize(); ptr_lhs++, ptr_rhs++) {
        if (!(lhs[ptr_lhs] == rhs[ptr_rhs])) {
            return false;
        }
    }
    return true;
}

template <typename TType>
inline bool operator!=(const SimpleVector<TType>& lhs, const SimpleVector<TType>& rhs) {
    return !(lhs == rhs);
}

template <typename TType>
inline bool operator<(const SimpleVector<TType>& lhs, const SimpleVector<TType>& rhs) {
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

template <typename TType>
inline bool operator<=(const SimpleVector<TType>& lhs, const SimpleVector<TType>& rhs) {
    return !(rhs < lhs);
}

template <typename TType>
inline bool operator>(const SimpleVector<TType>& lhs, const SimpleVector<TType>& rhs) {
    return rhs < lhs;
}

template <typename TType>
inline bool operator>=(const SimpleVector<TType>& lhs, const SimpleVector<TType>& rhs) {
    return !(lhs < rhs);
}



