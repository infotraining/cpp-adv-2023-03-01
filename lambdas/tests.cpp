#include "utils.hpp"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

using Utils::Gadget;

template <typename TContainer>
void print(const TContainer& container, const std::string& desc = "")
{
    std::cout << desc << ": [ ";
    for(typename TContainer::const_iterator it = container.begin(); it != container.end(); ++it) // ‘TContainer::const_iterator’ - ‘TContainer’ is a dependent scope
    {
        std::cout << *it << " ";
    }
    std::cout << "]\n";
}

template <typename Iterator, typename TPredicate>
Iterator my_find_if(Iterator begin, Iterator end, TPredicate predicate)
{
    for (Iterator pos = begin; pos != end; ++pos)
    {
        if (predicate(*pos))
            return pos;
    }

    return end;
}

bool is_even(int n)
{
    return n % 2 == 0;
}

bool is_less_evil(int n)
{
    return n == 665;
}

/////////////////////////////////////
// Functors or function objects

struct IsEven
{
    bool operator()(int n) const
    {
        return n % 2 == 0;
    }
};

struct IsDivisibleBy
{
    int n;

    bool operator()(int value) const
    {
        return value % n == 0;
    }
};

TEST_CASE("callables")
{
    IsEven check_even; // creation of an object

    REQUIRE(check_even(42) == true);
    REQUIRE(check_even(45) == false);

    IsDivisibleBy is_div_by_3{3};
    REQUIRE(is_div_by_3(9) == true);
    REQUIRE(is_div_by_3(4) == false);
}

TEST_CASE("lambda expression")
{
    std::vector<int> numbers = {1, 3, 7, 9, 42, 77, 665, 88 };

    auto pos_even = my_find_if(numbers.begin(), numbers.end(), is_even);

    if (pos_even != numbers.end())
    {
        std::cout << "Found even: " << *pos_even << "\n";
    }

    auto pos_less_evil = my_find_if(numbers.begin(), numbers.end(), is_less_evil);

    std::cout << "less evil: " << *pos_less_evil << "\n";

    auto pos_div_by_3 = my_find_if(numbers.begin(), numbers.end(), IsDivisibleBy{3});

    std::cout << "Count Div by 3: " << std::count_if(numbers.begin(), numbers.end(), IsDivisibleBy{3}) << "\n";

    std::vector<int> divs_by_3;

    std::copy_if(numbers.begin(), numbers.end(), std::back_inserter(divs_by_3), IsDivisibleBy{3});

    print(divs_by_3, "divs_by_3");
}