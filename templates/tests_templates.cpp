#include "utils.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <list>

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

// int* my_find(int* begin, int* end, int value)
// {
//     for(int* pos = begin; pos != end; ++pos)
//     {
//         if (*pos == value)
//             return pos;
//     }

//     return end;
// }

// std::vector<std::string>::iterator my_find(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end, std::string value)
// {
//     for(std::vector<std::string>::iterator pos = begin; pos != end; ++pos)
//     {
//         if (*pos == value)
//             return pos;
//     }

//     return end;
// }

template <typename Iterator, typename TValue>
Iterator my_find(Iterator begin, Iterator end, const TValue& value)
{
    for (Iterator pos = begin; pos != end; ++pos)
    {
        if (*pos == value)
            return pos;
    }

    return end;
}

template <typename Iterator, typename TTarget>
void my_pusher(Iterator begin, Iterator end, TTarget& target)
{
    for (Iterator pos = begin; pos != end; ++pos)
    {
        target.push_back(*pos);
    }
}


template <typename InputIterator, typename OutputIterator>
void my_copy(InputIterator begin, InputIterator end, OutputIterator target)
{
    for (InputIterator pos = begin; pos != end; ++pos)
    {
        *target = *pos;
        ++target;
    }
}

TEST_CASE("my_find - template function")
{
    int tab[10] = {1, 2, 4, 7, 3, 42, 7, 0, 1, 99};
    
    int tab_target[10];
    my_copy(std::begin(tab), std::end(tab), std::begin(tab_target));

    for(const auto& item : tab_target)
    {
        std::cout << item << " ";
    }
    std::cout << "\n";

    int* pos = std::find(tab, tab + 10, 42);
    REQUIRE(*pos == 42);

    pos = my_find(tab, tab + 10, 665);
    REQUIRE(pos == tab + 10);

    std::vector<std::string> words = {"one", "two", "three", "four"};

    std::vector<std::string>::iterator it = my_find(words.begin(), words.end(), std::string("three"));
    REQUIRE(*it == "three");

    it = my_find(words.begin(), words.end(), std::string("zero"));
    REQUIRE(it == words.end());

    std::list<int> numbers = {1, 2, 3, 665, 8};

    auto pos_less_evil = my_find(numbers.begin(), numbers.end(), 665);
    REQUIRE(*pos_less_evil == 665);

    std::vector<int> target(numbers.size());
    my_copy(numbers.begin(), numbers.end(), target.begin());
    REQUIRE(target == std::vector{1, 2, 3, 665, 8});

    my_pusher(numbers.begin(), numbers.end(), target);
    REQUIRE(target == std::vector{1, 2, 3, 665, 8, 1, 2, 3, 665, 8});
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