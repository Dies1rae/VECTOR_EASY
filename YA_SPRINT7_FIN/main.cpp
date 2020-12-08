#include "simple_vector.h"
#include "tests.h"

#include <cassert>
#include <iostream>
#include <numeric>

using namespace std;

class Xninja {
public:
    Xninja(): Xninja(5) {};

    Xninja(size_t num) :Xninja_(num) {};

    Xninja(const Xninja& other) = delete;

    Xninja& operator=(const Xninja& other) = delete;

    Xninja(Xninja&& other) {
        this->Xninja_ = exchange(other.Xninja_, 0);
    }

    Xninja& operator=(Xninja&& other) {
        this->Xninja_ = exchange(other.Xninja_, 0);
        return *this;
    }

    size_t GetXninja() const {
        return Xninja_;
    }

private:
    size_t Xninja_;
};

SimpleVector<int> GenerateVector(size_t size) {
    SimpleVector<int> test_vector(size);
    iota(test_vector.begin(), test_vector.end(), 1);
    return test_vector;
}

void TestTemporaryObjConstructor() {
    const size_t size = 1000000;
    cout << "Test with temporary object, copy elision" << endl;
    SimpleVector<int> moved_vector(GenerateVector(size));
    assert(moved_vector.GetSize() == size);
    cout << "Done!" << endl << endl;
}

void TestTemporaryObjOperator() {
    const size_t size = 1000000;
    cout << "Test with temporary object, operator=" << endl;
    SimpleVector<int> moved_vector;
    assert(moved_vector.GetSize() == 0);
    moved_vector = GenerateVector(size);
    assert(moved_vector.GetSize() == size);
    cout << "Done!" << endl << endl;
}

void TestNamedMoveConstructor() {
    const size_t size = 1000000;
    cout << "Test with named object, move constructor" << endl;
    SimpleVector<int> vector_to_move(GenerateVector(size));
    assert(vector_to_move.GetSize() == size);

    SimpleVector<int> moved_vector(move(vector_to_move));
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);
    cout << "Done!" << endl << endl;
}

void TestNamedMoveOperator() {
    const size_t size = 1000000;
    cout << "Test with named object, operator=" << endl;
    SimpleVector<int> vector_to_move(GenerateVector(size));
    assert(vector_to_move.GetSize() == size);

    SimpleVector<int> moved_vector = move(vector_to_move);
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);
    cout << "Done!" << endl << endl;
}

void TestNoncopiableMoveConstructor() {
    const size_t size = 5;
    cout << "Test noncopiable object, move constructor" << endl;
    SimpleVector<Xninja> vector_to_move;
    for (size_t i = 0; i < size; ++i) {
        vector_to_move.PushBack(Xninja(i));
    }

    SimpleVector<Xninja> moved_vector = move(vector_to_move);
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);

    for (size_t i = 0; i < size; ++i) {
        assert(moved_vector[i].GetXninja() == i);
    }
    cout << "Done!" << endl << endl;
}

void TestNoncopiablePushBack() {
    const size_t size = 5;
    cout << "Test noncopiable push back" << endl;
    SimpleVector<Xninja> test_vector;
    for (size_t i = 0; i < size; ++i) {
        test_vector.PushBack(Xninja(i));
    }

    assert(test_vector.GetSize() == size);

    for (size_t i = 0; i < size; ++i) {
        assert(test_vector[i].GetXninja() == i);
    }
    cout << "Done!" << endl << endl;
}

void TestNoncopiableInsert() {
    const size_t size = 5;
    cout << "Test noncopiable insert" << endl;
    SimpleVector<Xninja> test_vector;
    for (size_t i = 0; i < size; ++i) {
        test_vector.PushBack(Xninja(i));
    }

    // BEGIN
    test_vector.Insert(test_vector.begin(), Xninja(size + 1));
    assert(test_vector.GetSize() == size + 1);
    assert(test_vector.begin()->GetXninja() == size + 1);
    //END
    test_vector.Insert(test_vector.end(), Xninja(size + 2));
    assert(test_vector.GetSize() == size + 2);
    assert((test_vector.end() - 1)->GetXninja() == size + 2);
    //MIDDLE
    test_vector.Insert(test_vector.begin() + 3, Xninja(size + 3));
    assert(test_vector.GetSize() == size + 3);
    assert((test_vector.begin() + 3)->GetXninja() == size + 3);
    cout << "Done!" << endl << endl;
}

void TestNoncopiableErase() {
    const size_t size = 3;
    cout << "Test noncopiable erase" << endl;
    SimpleVector<Xninja> test_vector;
    for (size_t i = 0; i < size; ++i) {
        test_vector.PushBack(Xninja(i));
    }

    auto it = test_vector.Erase(test_vector.begin());
    assert(it->GetXninja() == 1);
    cout << "Done!" << endl << endl;
}

void AllTest() {
    cout << "Start!" << endl << endl;
    Test1();
    Test2();
    TestReserveConstructor();
    TestReserveMethod();
    TestTemporaryObjConstructor();
    TestTemporaryObjOperator();
    TestNamedMoveConstructor();
    TestNamedMoveOperator();
    TestNoncopiableMoveConstructor();
    TestNoncopiablePushBack();
    TestNoncopiableInsert();
    TestNoncopiableErase();
    cout << "All Done!" << endl << endl;
}

int main() {

    AllTest();

    return 0;
}