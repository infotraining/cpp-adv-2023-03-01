#include "utils.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using Utils::Gadget;

namespace WithoutTemplates
{
    int maximum(int a, int b)
    {
        return a < b ? b : a;
    }

    double maximum(double a, double b)
    {
        return a < b ? b : a;
    }
} // namespace WithoutTemplates

template <typename T>
T maximum(T a, T b)
{
    return a < b ? b : a;
}

// template <>
// const char* maximum<const char*>(const char* a, const char* b)
// {
//     return std::strcmp(a, b) < 0 ? b : a;
// }

const char* maximum(const char* a, const char* b)
{
    return std::strcmp(a, b) < 0 ? b : a;
}

struct Value
{
    int value;

    bool operator<(const Value& v) const
    {
        return value < v.value;
    }

    bool operator==(const Value& v) const
    {
        return value == v.value;
    }
};

TEST_CASE("function templates")
{
    int a = 10;
    int b = 665;
    REQUIRE(maximum(a, b) == 665);

    double da = 3.14;
    double db = 6.66;
    REQUIRE(maximum(da, db) == 6.66);

    std::string str_a = "abc";
    std::string str_b = "def";
    REQUIRE(maximum<std::string>(str_a, "def") == "def");

    REQUIRE(maximum(Value{10}, Value{20}) == Value{20});

    std::cout << maximum("Ala", "Ola") << "\n";

    int (*ptr_fun)(int, int) = &maximum<int>; // taking address of template function
}

TEST_CASE("my_find - template function")
{
    int tab[10] = {1, 2, 4, 7, 3, 42, 7, 0, 1, 99};

    int* pos = my_find(tab, tab+10, 42);
    REQUIRE(*pos == 42);

    pos = my_find(tab, tab+10, 665);
    REQUIRE(pos == tab+10);

    std::vector<std::string> words = {"one", "two", "three", "four"};

    std::vector<std::string>::iterator it = my_find(words.begin(), words.end(), std::string("three"));
    REQUIRE(*it == "three");

    it = my_find(words.begin(), words.end(), std::string("zero"));
    REQUIRE(it == words.end());
}


TEST_CASE("class templates")
{
    // TODO
}

TEST_CASE("template aliases")
{
    // TODO
}

TEST_CASE("template variables")
{
    // TODO
}