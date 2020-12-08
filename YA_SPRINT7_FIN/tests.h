#pragma once

#include <cassert>
#include <stdexcept>
#include <iostream>

using namespace std;

inline void Test1() {
    cout << "Basic vector functionality test!" << endl << endl;
    // ctor init def
    {
        SimpleVector<int> test_vector;
        assert(test_vector.GetSize() == 0u);
        assert(test_vector.IsEmpty());
        assert(test_vector.GetCapacity() == 0u);
    }

    // ctor with size
    {
        SimpleVector<int> test_vector(5);
        assert(test_vector.GetSize() == 5u);
        assert(test_vector.GetCapacity() == 5u);
        assert(!test_vector.IsEmpty());
        for (size_t i = 0; i < test_vector.GetSize(); ++i) {
            assert(test_vector[i] == 0);
        }
    }

    //ctor with size and item
    {
        SimpleVector<int> test_vector(3, 42);
        assert(test_vector.GetSize() == 3);
        assert(test_vector.GetCapacity() == 3);
        for (size_t i = 0; i < test_vector.GetSize(); ++i) {
            assert(test_vector[i] == 42);
        }
    }

    // ctor initializer_list
    {
        SimpleVector<int> test_vector{ 1, 2, 3 };
        assert(test_vector.GetSize() == 3);
        assert(test_vector.GetCapacity() == 3);
        assert(test_vector[2] == 3);
    }

    // At function
    {
        SimpleVector<int> test_vector(3);
        assert(&test_vector.At(2) == &test_vector[2]);
        try {
            test_vector.At(3);
            assert(false);  // !!!!!! out_of range
        } catch (const std::out_of_range&) {
        } catch (...) {
            assert(false);  // !!!!!!! NONO
        }
    }

    // clear
    {
        SimpleVector<int> test_vector(10);
        const size_t old_capacity = test_vector.GetCapacity();
        test_vector.Clear();
        assert(test_vector.GetSize() == 0);
        assert(test_vector.GetCapacity() == old_capacity);
    }

    // resize
    {
        SimpleVector<int> test_vector(3);
        test_vector[2] = 17;
        test_vector.Resize(7);
        assert(test_vector.GetSize() == 7);
        assert(test_vector.GetCapacity() >= test_vector.GetSize());
        assert(test_vector[2] == 17);
        assert(test_vector[3] == 0);
    }
    {
        SimpleVector<int> test_vector(3);
        test_vector[0] = 42;
        test_vector[1] = 55;
        const size_t old_capacity = test_vector.GetCapacity();
        test_vector.Resize(2);
        assert(test_vector.GetSize() == 2);
        assert(test_vector.GetCapacity() == old_capacity);
        assert(test_vector[0] == 42);
        assert(test_vector[1] == 55);
    }
    {
        const size_t old_size = 3;
        SimpleVector<int> test_vector(3);
        test_vector.Resize(old_size + 5);
        test_vector[3] = 42;
        test_vector.Resize(old_size);
        test_vector.Resize(old_size + 2);
        assert(test_vector[3] == 0);
    }

    // iterating
    {
        // empty
        {
            SimpleVector<int> test_vector;
            assert(test_vector.begin() == nullptr);
            assert(test_vector.end() == nullptr);
        }

        // not empty
        {
            SimpleVector<int> test_vector(10, 42);
            assert(test_vector.begin());
            assert(*test_vector.begin() == 42);
            assert(test_vector.end() == test_vector.begin() + test_vector.GetSize());
        }
    }
    cout << "Done!" << endl << endl;
}

inline void Test2() {
    cout << "Basic vector functionality test 2!" << endl << endl;
    // PushBack
    {
        SimpleVector<int> test_vector(1);
        test_vector.PushBack(42);
        assert(test_vector.GetSize() == 2);
        assert(test_vector.GetCapacity() >= test_vector.GetSize());
        assert(test_vector[0] == 0);
        assert(test_vector[1] == 42);
    }

    // Еpush_back no size++
    {
        SimpleVector<int> test_vector(2);
        test_vector.Resize(1);
        const size_t old_capacity = test_vector.GetCapacity();
        test_vector.PushBack(123);
        assert(test_vector.GetSize() == 2);
        assert(test_vector.GetCapacity() == old_capacity);
    }

    // PopBack
    {
        SimpleVector<int> test_vector{ 0, 1, 2, 3 };
        const size_t old_capacity = test_vector.GetCapacity();
        const auto old_begin = test_vector.begin();
        test_vector.PopBack();
        assert(test_vector.GetCapacity() == old_capacity);
        assert(test_vector.begin() == old_begin);
        assert((test_vector == SimpleVector<int>{0, 1, 2}));
    }

    // ctor copied
    {
        SimpleVector<int> numbers{ 1, 2 };
        auto numbers_copy(numbers);
        assert(&numbers_copy[0] != &numbers[0]);
        assert(numbers_copy.GetSize() == numbers.GetSize());
        for (size_t i = 0; i < numbers.GetSize(); ++i) {
            assert(numbers_copy[i] == numbers[i]);
            assert(&numbers_copy[i] != &numbers[i]);
        }
    }

    // asserst with logic
    {
        assert((SimpleVector{ 1, 2, 3 } == SimpleVector{ 1, 2, 3 }));
        assert((SimpleVector{ 1, 2, 3 } != SimpleVector{ 1, 2, 2 }));

        assert((SimpleVector{ 1, 2, 3 } < SimpleVector{ 1, 2, 3, 1 }));
        assert((SimpleVector{ 1, 2, 3 } > SimpleVector{ 1, 2, 2, 1 }));

        assert((SimpleVector{ 1, 2, 3 } >= SimpleVector{ 1, 2, 3 }));
        assert((SimpleVector{ 1, 2, 4 } >= SimpleVector{ 1, 2, 3 }));
        assert((SimpleVector{ 1, 2, 3 } <= SimpleVector{ 1, 2, 3 }));
        assert((SimpleVector{ 1, 2, 3 } <= SimpleVector{ 1, 2, 4 }));
    }

    // swap
    {
        SimpleVector<int> test_vector1{ 42, 666 };
        SimpleVector<int> test_vector2;
        test_vector2.PushBack(0);
        test_vector2.PushBack(1);
        test_vector2.PushBack(2);
        const int* const begin1 = &test_vector1[0];
        const int* const begin2 = &test_vector2[0];

        const size_t capacity1 = test_vector1.GetCapacity();
        const size_t capacity2 = test_vector2.GetCapacity();

        const size_t size1 = test_vector1.GetSize();
        const size_t size2 = test_vector2.GetSize();

        static_assert(noexcept(test_vector1.swap(test_vector2)));

        test_vector1.swap(test_vector2);

        assert(&test_vector2[0] == begin1);
        assert(&test_vector1[0] == begin2);
        assert(test_vector1.GetSize() == size2);
        assert(test_vector2.GetSize() == size1);
        assert(test_vector1.GetCapacity() == capacity2);
        assert(test_vector2.GetCapacity() == capacity1);
    }

    // operator =
    {
        SimpleVector<int> src_vector{ 1, 2, 3, 4 };
        SimpleVector<int> dst_vector{ 1, 2, 3, 4, 5, 6 };
        dst_vector = src_vector;
        assert(dst_vector == src_vector);
    }

    //insert in beg mid end
    {
        SimpleVector<int> test_vector{ 1, 2, 3, 4 };
        SimpleVector<int> test_vector1;
        SimpleVector<int> test_vector2{ 1, 2, 3, 4 };
        SimpleVector<int> test_vector3{ 1, 2, 3, 4 };
        test_vector2.PopBack();
        test_vector2.PopBack();
       
        test_vector3.Insert(test_vector3.begin() + test_vector3.GetSize(), 42);
        assert((test_vector3 == SimpleVector<int>{1, 2, 3, 4, 42}));

        test_vector.Insert(test_vector.begin() + 2, 42);
        assert((test_vector == SimpleVector<int>{1, 2, 42, 3, 4}));

        test_vector1.Insert(test_vector1.begin(), 42);
        assert((test_vector1 == SimpleVector<int>{42}));

        test_vector2.Insert(test_vector2.begin() + 1, 42);
        assert((test_vector2 == SimpleVector<int>{1, 42, 2}));
    }


    {
        SimpleVector<int> test_vector{ 1, 2, 3, 4 };

        test_vector.Erase(test_vector.cbegin() + 2);

        assert((test_vector == SimpleVector<int>{1, 2, 4}));
    }
    cout << "Done!" << endl << endl;
}

void TestReserveConstructor() {
    cout << "TestReserveConstructor"s << endl;
    SimpleVector<int> test_vector(Reserve(5));
    assert(test_vector.GetCapacity() == 5);
    assert(test_vector.IsEmpty());
    cout << "Done!"s << endl;
}

void TestReserveMethod() {
    cout << "TestReserveMethod"s << endl;
    SimpleVector<int> test_vector;
    // зарезервируем 5 мест в векторе
    test_vector.Reserve(5);
    assert(test_vector.GetCapacity() == 5);
    assert(test_vector.IsEmpty());

    // попытаемся уменьшить capacity до 1
    test_vector.Reserve(1);
    // capacity должно остаться прежним
    assert(test_vector.GetCapacity() == 5);
    // поместим 10 элементов в вектор
    for (int i = 0; i < 10; ++i) {
        test_vector.PushBack(i);
    }
    assert(test_vector.GetSize() == 10);
    // увеличим capacity до 100
    test_vector.Reserve(100);
    // проверим, что размер не поменялся
    assert(test_vector.GetSize() == 10);
    assert(test_vector.GetCapacity() == 100);
    // проверим, что элементы на месте
    for (int i = 0; i < 10; ++i) {
        assert(test_vector[i] == i);
    }
    cout << "Done!"s << endl;
}