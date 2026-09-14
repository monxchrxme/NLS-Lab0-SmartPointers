#include <gtest/gtest.h>
#include "UnqPtr.hpp"
#include "ShrdPtr.hpp" 
#include "SmrtPtr.hpp" 
#include "MemorySpan.hpp" 
#include "DynamicArraySequence.hpp" 

// Вспомогательный класс для точного контроля утечек памяти
struct TestObject {
    static inline size_t alive_count = 0;
    int value;

    explicit TestObject(int v = 0) : value(v) {
        ++alive_count;
    }
    ~TestObject() {
        --alive_count;
    }
};

// Иерархия классов для проверки полиморфизма
struct Base {
    static inline size_t base_alive = 0;
    explicit Base() { ++base_alive; }
    virtual ~Base() { --base_alive; }
    virtual int get_type() const { return 1; }
};

struct Derived : public Base {
    static inline size_t derived_alive = 0;
    explicit Derived() { ++derived_alive; }
    ~Derived() override { --derived_alive; }
    int get_type() const override { return 2; }
};

// ============================================================================
// Тесты UnqPtr<T>
// ============================================================================

TEST(UnqPtrTest, DefaultConstruction) {
    custom::UnqPtr<int> ptr;
    EXPECT_EQ(ptr.get(), nullptr);
    EXPECT_FALSE(ptr);
}

TEST(UnqPtrTest, ValueConstructionAndAccess) {
    custom::UnqPtr<int> ptr(new int(42));
    ASSERT_NE(ptr.get(), nullptr);
    EXPECT_TRUE(ptr);
    EXPECT_EQ(*ptr, 42);

    *ptr = 100;
    EXPECT_EQ(*ptr, 100);
}

TEST(UnqPtrTest, DestructorCleansUpMemory) {
    ASSERT_EQ(TestObject::alive_count, 0);
    {
        auto ptr = custom::MakeUnq<TestObject>(10);
        EXPECT_EQ(TestObject::alive_count, 1);
        EXPECT_EQ(ptr->value, 10);
    } 
    EXPECT_EQ(TestObject::alive_count, 0);
}

TEST(UnqPtrTest, MoveSemantics) {
    ASSERT_EQ(TestObject::alive_count, 0);
    {
        custom::UnqPtr<TestObject> ptr1(new TestObject(55));
        EXPECT_EQ(TestObject::alive_count, 1);

        // Move конструктор
        custom::UnqPtr<TestObject> ptr2 = std::move(ptr1);
        EXPECT_EQ(ptr1.get(), nullptr);
        ASSERT_NE(ptr2.get(), nullptr);
        EXPECT_EQ(ptr2->value, 55);
        EXPECT_EQ(TestObject::alive_count, 1);

        // Move присваивание
        custom::UnqPtr<TestObject> ptr3;
        ptr3 = std::move(ptr2);
        EXPECT_EQ(ptr2.get(), nullptr);
        EXPECT_EQ(ptr3->value, 55);
        EXPECT_EQ(TestObject::alive_count, 1);
    }
    EXPECT_EQ(TestObject::alive_count, 0);
}

TEST(UnqPtrTest, ResetAndRelease) {
    ASSERT_EQ(TestObject::alive_count, 0);
    {
        custom::UnqPtr<TestObject> ptr(new TestObject(1));
        EXPECT_EQ(TestObject::alive_count, 1);

        // release() отдает владение без уничтожения
        TestObject* raw = ptr.release();
        EXPECT_EQ(ptr.get(), nullptr);
        EXPECT_EQ(TestObject::alive_count, 1);

        // reset() уничтожает старый объект и берет новый
        ptr.reset(raw);
        EXPECT_EQ(TestObject::alive_count, 1);

        ptr.reset(new TestObject(2));
        EXPECT_EQ(TestObject::alive_count, 1);
        EXPECT_EQ(ptr->value, 2);

        ptr.reset(nullptr);
        EXPECT_EQ(TestObject::alive_count, 0);
    }
}

// ============================================================================
// Тесты подтипизации (Upcasting: Derived -> Base)
// ============================================================================

TEST(UnqPtrTest, SubtypingAndVirtualDestruction) {
    ASSERT_EQ(Base::base_alive, 0);
    ASSERT_EQ(Derived::derived_alive, 0);

    {
        custom::UnqPtr<Derived> derived_ptr(new Derived());
        EXPECT_EQ(Base::base_alive, 1);
        EXPECT_EQ(Derived::derived_alive, 1);

        // Перемещение Derived в Base
        custom::UnqPtr<Base> base_ptr = std::move(derived_ptr);
        EXPECT_EQ(derived_ptr.get(), nullptr);
        ASSERT_NE(base_ptr.get(), nullptr);
        EXPECT_EQ(base_ptr->get_type(), 2); // Виртуальный вызов метода Derived

        // Живы оба уровня иерархии
        EXPECT_EQ(Base::base_alive, 1);
        EXPECT_EQ(Derived::derived_alive, 1);
    }

    // При выходе из области видимости виртуальный деструктор Base обязан
    // уничтожить и Derived, и Base
    EXPECT_EQ(Derived::derived_alive, 0);
    EXPECT_EQ(Base::base_alive, 0);
}

// ============================================================================
// Тесты специализации для массивов UnqPtr<T[]>
// ============================================================================

TEST(UnqPtrArrayTest, ArraySubscriptAndDestruction) {
    ASSERT_EQ(TestObject::alive_count, 0);
    const size_t array_size = 5;

    {
        // Выделение массива элементов
        auto arr = custom::MakeUnq<TestObject[]>(array_size);
        EXPECT_EQ(TestObject::alive_count, array_size);

        for (size_t i = 0; i < array_size; ++i) {
            arr[i].value = static_cast<int>(i * 10);
        }

        for (size_t i = 0; i < array_size; ++i) {
            EXPECT_EQ(arr[i].value, static_cast<int>(i * 10));
        }
    } 

    EXPECT_EQ(TestObject::alive_count, 0);
}

// ============================================================================
// Тесты ShrdPtr<T>
// ============================================================================

TEST(ShrdPtrTest, NullptrAndUseCount) {
    custom::ShrdPtr<int> empty;
    EXPECT_EQ(empty.get(), nullptr);
    EXPECT_EQ(empty.use_count(), 0);
    EXPECT_FALSE(empty);
    EXPECT_TRUE(empty == nullptr);
    EXPECT_FALSE(empty != nullptr);
}

TEST(ShrdPtrTest, SharedOwnershipAndLifetime) {
    ASSERT_EQ(TestObject::alive_count, 0);
    {
        auto sp1 = custom::MakeShrd<TestObject>(100);
        EXPECT_EQ(TestObject::alive_count, 1);
        EXPECT_EQ(sp1.use_count(), 1);
        EXPECT_EQ(sp1->value, 100);

        {
            // увеличиваем счетчик
            custom::ShrdPtr<TestObject> sp2 = sp1;
            EXPECT_EQ(sp1.use_count(), 2);
            EXPECT_EQ(sp2.use_count(), 2);
            EXPECT_EQ(TestObject::alive_count, 1);

            {
                custom::ShrdPtr<TestObject> sp3;
                sp3 = sp2;
                EXPECT_EQ(sp1.use_count(), 3);
                EXPECT_EQ(TestObject::alive_count, 1);
            } // sp3 уничтожен
            EXPECT_EQ(sp1.use_count(), 2);
            EXPECT_EQ(TestObject::alive_count, 1);
        } // sp2 уничтожен
        EXPECT_EQ(sp1.use_count(), 1);
        EXPECT_EQ(TestObject::alive_count, 1);
    } // sp1 уничтожен: объект обязан удалиться

    EXPECT_EQ(TestObject::alive_count, 0);
}

TEST(ShrdPtrTest, MoveSemantics) {
    ASSERT_EQ(TestObject::alive_count, 0);
    {
        auto sp1 = custom::MakeShrd<TestObject>(42);
        EXPECT_EQ(sp1.use_count(), 1);

        // Move конструктор: счетчик не должен меняться
        custom::ShrdPtr<TestObject> sp2 = std::move(sp1);
        EXPECT_EQ(sp1.get(), nullptr);
        EXPECT_EQ(sp1.use_count(), 0);
        EXPECT_EQ(sp2.use_count(), 1);
        EXPECT_EQ(sp2->value, 42);
        EXPECT_EQ(TestObject::alive_count, 1);

        // Move присваивание
        custom::ShrdPtr<TestObject> sp3;
        sp3 = std::move(sp2);
        EXPECT_EQ(sp2.get(), nullptr);
        EXPECT_EQ(sp3.use_count(), 1);
        EXPECT_EQ(TestObject::alive_count, 1);
    }
    EXPECT_EQ(TestObject::alive_count, 0);
}

TEST(ShrdPtrTest, SubtypingPolymorphism) {
    ASSERT_EQ(Base::base_alive, 0);
    ASSERT_EQ(Derived::derived_alive, 0);
    {
        auto derived_ptr = custom::MakeShrd<Derived>();
        EXPECT_EQ(Base::base_alive, 1);
        EXPECT_EQ(Derived::derived_alive, 1);
        EXPECT_EQ(derived_ptr.use_count(), 1);

        // Upcasting: копируем ShrdPtr<Derived> в ShrdPtr<Base>
        custom::ShrdPtr<Base> base_ptr = derived_ptr;
        EXPECT_EQ(derived_ptr.use_count(), 2);
        EXPECT_EQ(base_ptr.use_count(), 2);
        EXPECT_EQ(base_ptr->get_type(), 2); // Полиморфный вызов
    }
    EXPECT_EQ(Derived::derived_alive, 0);
    EXPECT_EQ(Base::base_alive, 0);
}

TEST(ShrdPtrArrayTest, ArraySubscriptAndDestruction) {
    ASSERT_EQ(TestObject::alive_count, 0);
    const size_t size = 4;
    {
        auto arr1 = custom::MakeShrd<TestObject[]>(size);
        EXPECT_EQ(TestObject::alive_count, size);
        EXPECT_EQ(arr1.use_count(), 1);

        for (size_t i = 0; i < size; ++i) {
            arr1[i].value = static_cast<int>(i + 1);
        }

        {
            custom::ShrdPtr<TestObject[]> arr2 = arr1;
            EXPECT_EQ(arr1.use_count(), 2);
            EXPECT_EQ(arr2[2].value, 3);
        }
        EXPECT_EQ(arr1.use_count(), 1);
        EXPECT_EQ(TestObject::alive_count, size);
    }
    EXPECT_EQ(TestObject::alive_count, 0);
}

// ============================================================================
// Тесты SmrtPtr<T> и CentralStorage
// ============================================================================

TEST(SmrtPtrTest, CentralStorageLifecycleAndLeakCheck) {
    EXPECT_EQ(custom::CentralStorage::instance().tracked_objects_count(), 0);
    ASSERT_EQ(TestObject::alive_count, 0);

    {
        auto p1 = custom::MakeSmrt<TestObject>(77);
        EXPECT_EQ(TestObject::alive_count, 1);
        EXPECT_EQ(p1.use_count(), 1);
        EXPECT_EQ(custom::CentralStorage::instance().tracked_objects_count(), 1);

        {
            // Копирование через реестр
            custom::SmrtPtr<TestObject> p2 = p1;
            EXPECT_EQ(p1.use_count(), 2);
            EXPECT_EQ(p2.use_count(), 2);
            EXPECT_EQ(custom::CentralStorage::instance().tracked_objects_count(), 1);

            // Move-семантика
            custom::SmrtPtr<TestObject> p3 = std::move(p2);
            EXPECT_EQ(p2.get(), nullptr);
            EXPECT_EQ(p3.use_count(), 2);
            EXPECT_EQ(p1.use_count(), 2);
        } // p3 уничтожен, p2 пустой

        EXPECT_EQ(p1.use_count(), 1);
        EXPECT_EQ(TestObject::alive_count, 1);
        EXPECT_EQ(custom::CentralStorage::instance().tracked_objects_count(), 1);
    } // p1 уничтожен: объект обязан удалиться, а запись из таблицы стереться

    EXPECT_EQ(TestObject::alive_count, 0);
    // Проверка отсутствия утечек в самом реестре:
    EXPECT_EQ(custom::CentralStorage::instance().tracked_objects_count(), 0);
}

TEST(SmrtPtrTest, SubtypingPolymorphism) {
    ASSERT_EQ(Base::base_alive, 0);
    ASSERT_EQ(Derived::derived_alive, 0);
    {
        auto derived_smrt = custom::MakeSmrt<Derived>();
        EXPECT_EQ(derived_smrt.use_count(), 1);

        // Upcasting в SmrtPtr<Base>
        custom::SmrtPtr<Base> base_smrt = derived_smrt;
        EXPECT_EQ(derived_smrt.use_count(), 2);
        EXPECT_EQ(base_smrt.use_count(), 2);
        EXPECT_EQ(base_smrt->get_type(), 2);
    }
    EXPECT_EQ(Derived::derived_alive, 0);
    EXPECT_EQ(Base::base_alive, 0);
    EXPECT_EQ(custom::CentralStorage::instance().tracked_objects_count(), 0);
}

TEST(SmrtPtrTest, MemoryFootprintComparison) {
    // SmrtPtr весит ровно столько же, сколько сырой указатель (8 байт)
    EXPECT_EQ(sizeof(custom::SmrtPtr<int>), sizeof(int*));
    // ShrdPtr весит 16 байт (2 указателя: на объект и на счетчик)
    EXPECT_EQ(sizeof(custom::ShrdPtr<int>), sizeof(int*) * 2);
}

// ============================================================================
// Тесты MemorySpan<T> и MsPtr<T>
// ============================================================================

TEST(MemorySpanTest, CreationAndMethods) {
    custom::MemorySpan<int> span(5, 100);
    EXPECT_EQ(span.size(), 5);

    // Get возвращает UnqPtr
    auto unq = span.Get(2);
    EXPECT_EQ(*unq, 100);

    // Copy возвращает ShrdPtr
    auto shrd = span.Copy(2);
    EXPECT_EQ(*shrd, 100);
    EXPECT_EQ(shrd.use_count(), 1);

    // Проверка выхода за границы Get и Copy
    EXPECT_THROW(span.Get(10), std::out_of_range);
    EXPECT_THROW(span.Copy(10), std::out_of_range);
}

TEST(MsPtrTest, PointerArithmeticAndBoundsSafety) {
    custom::MemorySpan<int> span(3);
    auto it = span.begin();
    *it = 10;
    *(it + 1) = 20;
    *(it + 2) = 30;

    EXPECT_EQ(*it, 10);
    EXPECT_EQ(*(it + 1), 20);
    EXPECT_EQ(it[2], 30);

    ++it;
    EXPECT_EQ(*it, 20);
    it++;
    EXPECT_EQ(*it, 30);

    ++it;
    EXPECT_TRUE(it == span.end());
    EXPECT_THROW(*it, std::out_of_range);

    EXPECT_THROW(++it, std::out_of_range);

    // Возврат назад
    auto back = span.end();
    --back;
    EXPECT_EQ(*back, 30);
    --back;
    EXPECT_EQ(*back, 20);
    --back;
    EXPECT_EQ(*back, 10);

    EXPECT_THROW(--back, std::out_of_range);
}

TEST(MsPtrTest, PointerDifferenceAndComparisons) {
    custom::MemorySpan<int> span(5);
    auto p1 = span.Locate(1);
    auto p4 = span.Locate(4);

    EXPECT_EQ(p4 - p1, 3);
    EXPECT_EQ(p1 - p4, -3);

    EXPECT_TRUE(p1 < p4);
    EXPECT_TRUE(p4 > p1);
    EXPECT_FALSE(p1 == p4);

    // Защита от операций над разными буферами
    custom::MemorySpan<int> other_span(5);
    auto foreign_ptr = other_span.begin();

    EXPECT_THROW(p1 - foreign_ptr, std::invalid_argument);
    EXPECT_THROW(p1 < foreign_ptr, std::invalid_argument);
}

// ============================================================================
// Тесты DynamicArraySequence<T>
// ============================================================================

TEST(SequenceTest, BasicOperationsAndGrowth) {
    custom::DynamicArraySequence<int> seq;
    EXPECT_TRUE(seq.IsEmpty());
    EXPECT_EQ(seq.GetLength(), 0);

    seq.Append(10);
    seq.Append(20);
    seq.Append(30);

    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_EQ(seq.GetFirst(), 10);
    EXPECT_EQ(seq.GetLast(), 30);
    EXPECT_EQ(seq.Get(1), 20);

    seq.Prepend(5);
    EXPECT_EQ(seq.GetFirst(), 5);
    EXPECT_EQ(seq.GetLength(), 4);

    seq.InsertAt(15, 2); // 5, 10, 15, 20, 30
    EXPECT_EQ(seq.Get(2), 15);
    EXPECT_EQ(seq.GetLength(), 5);
}

TEST(SequenceTest, SmartPointerExport) {
    custom::DynamicArraySequence<int> seq;
    seq.Append(42);

    auto unq = seq.GetUnique(0);
    EXPECT_EQ(*unq, 42);

    auto shrd = seq.GetShared(0);
    EXPECT_EQ(*shrd, 42);
    EXPECT_EQ(shrd.use_count(), 1);
}

TEST(SequenceTest, ExceptionSafety) {
    custom::DynamicArraySequence<int> seq;
    EXPECT_THROW(seq.GetFirst(), std::out_of_range);
    EXPECT_THROW(seq.GetLast(), std::out_of_range);
    EXPECT_THROW(seq.Get(10), std::out_of_range);
    EXPECT_THROW(seq.InsertAt(99, 10), std::out_of_range);
}

TEST(SequenceTest, ZeroLeaksWithTestObject) {
    ASSERT_EQ(TestObject::alive_count, 0);
    {
        custom::DynamicArraySequence<TestObject> seq;
        for (int i = 0; i < 50; ++i) {
            seq.Append(TestObject(i));
        }
        EXPECT_EQ(seq.GetLength(), 50);
        EXPECT_EQ(seq.GetCapacity(), 64);
        EXPECT_EQ(TestObject::alive_count, seq.GetCapacity());
    } 

    EXPECT_EQ(TestObject::alive_count, 0);
}