#include "Type.hpp"
#include <gtest/gtest.h>
#include <string>
using namespace MEngine;
TEST(PropertyTest, basic)
{
    Property<int> a;
    EXPECT_EQ(a, 0);
    a.Set(2);
    EXPECT_EQ(a, 2);
    int value = 3;
    a.Set(value);
    EXPECT_EQ(a, 3);
}
TEST(PropertyTest, customGetterSetter)
{
    // 自定义getter
    Property<int> a1{[](const int &value) { return value + 2; }};
    a1 = 3;
    EXPECT_EQ(a1, 5);
    Property<int> a2{3, [](const int &value) { return value + 2; }};
    EXPECT_EQ(a2, 5);
    // 自定义setter
    Property<int> b1{[](int &value, const int &newValue) { value = newValue + 2; }};
    b1 = 1;
    EXPECT_EQ(b1, 3);
    Property<int> b2{1, {}, [](int &value, const int &newValue) { value = newValue + 2; }};
    EXPECT_EQ(b2, 3);
}
TEST(PropertyTest, customGetterSetter2)
{
    class TestClass
    {
      public:
        Property<int> value{[this](int &value, const int &newValue) { SetValue(value, newValue); }};
        void SetValue(int &dst, const int &newValue)
        {
            dst = newValue + 2;
        }
    };
}
TEST(PropertyTest, construct)
{
    Property<int> a1{3};
    Property<int> a2 = a1;
    EXPECT_EQ(a1, a2);
    Property<int> a3 = std::move(a1);
    EXPECT_EQ(a3, 3);
    EXPECT_EQ(a1, 0);

    Property<int> a4{3, [](const int &value) { return value + 2; }};
    Property<int> a5 = a4;
    EXPECT_EQ(a4, a5);

    Property<int> a6 = std::move(a4);
    EXPECT_EQ(a6, 5);
    EXPECT_EQ(a4, 0);

    Property<int> a7{3, {}, [](int &value, const int &newValue) { value = newValue + 2; }};
    Property<int> a8{a7};
    EXPECT_EQ(a7, a8);
}