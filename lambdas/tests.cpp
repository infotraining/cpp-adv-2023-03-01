#include "utils.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using Utils::Gadget;

template <typename TContainer>
void print(const TContainer& container, const std::string& desc = "")
{
    std::cout << desc << ": [ ";
    for (typename TContainer::const_iterator it = container.begin(); it != container.end(); ++it) // ‘TContainer::const_iterator’ - ‘TContainer’ is a dependent scope
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
    SECTION("functions or function pointer")
    {
        REQUIRE(is_even(42) == true);

        bool (*predicate)(int) = is_even;

        REQUIRE(predicate(42));
    }

    SECTION("functors aka. function objects")
    {
        IsEven check_even; // creation of an object

        REQUIRE(check_even(42) == true);
        REQUIRE(check_even(45) == false);

        IsDivisibleBy is_div_by_3{3};
        REQUIRE(is_div_by_3(9) == true);
        REQUIRE(is_div_by_3(4) == false);
    }

    SECTION("lambda")
    {
        auto is_even_lambda = [](int n) {
            return n % 2 == 0;
        };

        REQUIRE(is_even_lambda(42));
    }
}

TEST_CASE("lambda expression")
{
    std::vector<int> numbers = {1, 3, 7, 9, 42, 77, 665, 88};

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

// closure class
class Lambda_7286345283645
{
public:
    auto operator()(int n) const { return n % 2 == 0; }
};

TEST_CASE("lambda - how it works")
{
    auto is_even_lambda = [](int n) {
        return n % 2 == 0;
    };

    SECTION("is interpreted")
    {
        auto is_even_lambda = Lambda_7286345283645{};
    }

    REQUIRE(is_even_lambda(665) == false);
}

struct Person
{
    std::string name;
    uint8_t age;

    bool operator<(const Person& other) const
    {
        return name < other.name;
    }
};

std::ostream& operator<<(std::ostream& out, const Person& p)
{
    out << "Person{" << p.name << ", " << static_cast<int>(p.age) << "}";
    return out; 
}

TEST_CASE("algorithms with lambda")
{
    std::vector<int> numbers = {1, 103, 7, 9, 42, 77, 665, 188};

    std::cout << "Count > 100: " << std::count_if(numbers.begin(), numbers.end(), [](int n) { return n > 100; });

    std::vector<Person> people = {{"Kowalski", 11}, {"Nowak", 15}, {"Adamski", 67}, {"Anonim", 8}};

    std::cout << "Adults: " << std::count_if(people.begin(), people.end(), [](const Person& p) { return p.age > 18; }) << "\n";

    std::sort(people.begin(), people.end());
    print(people, "people by name");

    std::sort(people.begin(), people.end(), [](const Person& a, const Person& b) { return a.age < b.age; });
    print(people, "people by age");

    int age_for_retirement = 65;

    std::cout << "Retired: " << std::count_if(people.begin(), people.end(), [age_for_retirement](const Person& p) { return p.age > age_for_retirement; }) << "\n";


    int factor = 2;
    std::vector<int> scaled_by_2;

    std::for_each(numbers.begin(), numbers.end(), [factor, &scaled_by_2](int n) { scaled_by_2.push_back(n * factor); });

    //std::for_each(numbers.begin(), numbers.end(), [=, &scaled_by_2](int n) { scaled_by_2.push_back(n * factor); }); // [=, &scaled_by_2] - default capture by value, scaled_by_2 by ref

    // std::for_each(numbers.begin(), numbers.end(), [&, factor](int n) { scaled_by_2.push_back(n * factor); }); // [&, factor] - default capture by ref, factor by value

    print(scaled_by_2, "scaled_by_2");
}