#include "utils.hpp"

#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <cstring>

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

    for (const auto& item : tab_target)
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

// template <typename T>
// T my_exchange(T& old_value, T new_value)
// {
//     T temp = std::move(old_value);
//     old_value = std::move(new_value);
//     return temp;
// }

namespace ver_1_0
{
    template <typename T, typename U>
    T my_exchange(T& old_value, U new_value)
    {
        T value = std::move(old_value);
        old_value = std::move(new_value);
        return value;
    }
} // namespace ver_1_0

template <typename T, typename U>
T my_exchange(T& old_value, U&& new_value)
{
    T value = std::move(old_value);
    old_value = std::forward<U>(new_value);
    return value;
}

namespace Explain
{
    void set(std::string& value, const std::string& new_value)
    {
        value = new_value; // copy of new_value to value
    }

    void set(std::string& value, std::string&& new_value)
    {
        value = std::move(new_value);
    }
} // namespace Explain

template <typename T, typename U>
void set(T& value, U&& new_value /*universal reference*/)
{
    value = std::forward<U>(new_value); /*prefect forwarding*/
}

TEST_CASE("set")
{
    using namespace std::literals;

    std::string str;
    std::string value = "text";

    set(str, value);
    REQUIRE(str == "text");

    set(str, "another text"s);
    REQUIRE(str == "another text"s);
}

TEST_CASE("exchange")
{
    SECTION("int")
    {
        SECTION("lvalue")
        {
            int x = 10;
            int new_value = 42;
            int y = my_exchange(x, new_value);

            REQUIRE(y == 10);
            REQUIRE(x == 42);
            REQUIRE(new_value == 42);
        }

        SECTION("rvalue")
        {
            int x = 10;
            int y = my_exchange(x, 42);

            REQUIRE(y == 10);
            REQUIRE(x == 42);
        }
    }

    SECTION("string")
    {
        using namespace std::literals;
        SECTION("lvalue")
        {
            std::string s1 = "one";
            std::string new_value = "two";
            std::string s2 = my_exchange(s1, new_value);

            REQUIRE(s2 == "one");
            REQUIRE(s1 == "two");
            REQUIRE(new_value == "two");
        }

        SECTION("rvalue")
        {
            std::string s1 = "one";
            std::string s2 = my_exchange(s1, "two"s);

            REQUIRE(s2 == "one");
            REQUIRE(s1 == "two");
        }
    }
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec)
{
    out << "std::vector{ ";
    for (const auto& item : vec)
        out << item << " ";
    out << "}";

    return out;
}

template <typename T1, typename T2>
class ValuePair
{
    T1 first_;
    T2 second_;

public:
    ValuePair(T1 fst, T2 snd)
        : first_{std::move(fst)}
        , second_{std::move(snd)}
    { }

    T1& first() // read-write
    {
        return first_;
    }

    const T1& first() const // read-only
    {
        return first_;
    }

    T2& second()
    {
        return second_;
    }

    const T2& second() const
    {
        return second_;
    }

    std::string to_string() const;
};

template <typename T1, typename T2>
std::string ValuePair<T1, T2>::to_string() const
{
    std::stringstream ss;
    ss << "[ " << first_ << ", " << second_ << "]";
    return ss.str();
}

// partial specialization
template <typename T>
class ValuePair<T, T>
{
    T items_[2];

public:
    ValuePair(T fst, T snd)
        : items_{std::move(fst), std::move(snd)}
    { }

    T& first() // read-write
    {
        return items_[0];
    }

    const T& first() const // read-only
    {
        return items_[0];
    }

    T& second()
    {
        return items_[1];
    }

    const T& second() const
    {
        return items_[1];
    }

    const T& maximum() const
    {
        return first() < second() ? second() : first();
    }

    std::string to_string() const
    {
        std::stringstream ss;
        ss << "[ " << first() << ", " << second() << "]";
        return ss.str();
    }
};

void print(const ValuePair<int, std::string>& vp)
{
    std::cout << "ValuePair: " << vp.to_string() << "\n";
}

TEST_CASE("class templates")
{
    using namespace std::literals;

    ValuePair<int, std::string> vp1{1, "text"s};
    REQUIRE(vp1.first() == 1);
    REQUIRE(vp1.second() == "text"s);

    ValuePair<std::string, std::vector<int>> vp2("dataset", std::vector<int>{1, 2, 3, 4});
    REQUIRE(vp2.first() == "dataset"s);
    REQUIRE(vp2.second() == std::vector<int>{1, 2, 3, 4});

    std::cout << vp2.to_string() << "\n";

    ValuePair<int, int> vp3{42, 665};
    std::cout << "vp3: " << vp3.to_string() << " - max: " << vp3.maximum() << "\n";
}

template <typename T, size_t N>
struct Array
{
    T items[N];

    using iterator = T*;
    using const_iterator = const T*;
    using reference = T&;
    using const_reference = const T&;

    size_t size() const
    {
        return N;
    }

    iterator begin()
    {
        return items;
    }

    iterator end()
    {
        return items + N;
    }

    const_iterator begin() const
    {
        return items;
    }

    const_iterator end() const
    {
        return items + N;
    }

    reference operator[](size_t index)
    {
        return items[index];
    }

    const_reference operator[](size_t index) const
    {
        return items[index];
    }
};

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


TEST_CASE("template parameters")
{
    const Array<int, 10> arr1{1, 2, 3, 4, 5};

    REQUIRE(arr1.size() == 10);

    // arr1[7] = 7;
    int value = arr1[3];

    for (const auto& item : arr1)
        std::cout << item << " ";
    std::cout << "\n";

    print(arr1, "arr1");
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
using Dictionary = std::map<std::string, T>;

template <typename T>
using DictionaryDesc = std::map<std::string, T, std::greater<T>>;

template <typename T>
using Buffer = Array<T, 1024>;

template <typename T>
using LargeBuffer = Array<T, 2048>;

template <typename TDict = Dictionary<std::string>>
class Translator
{
    TDict dict;
};

TEST_CASE("template aliases")
{
    Dictionary<int> dict = { {"one", 1}, {"two", 2} };

    Buffer<uint8_t> buffer = {};
    LargeBuffer<uint8_t> large_buffer = {};
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

const double pi_d = 3.141592653589793238462643383279;
const float pi_f = 3.141592653589793238462643383279;

template <typename T>
const T pi = 3.141592653589793238462643383279;

TEST_CASE("template variables")
{
    std::cout << "pi: " << pi<double> << "\n";
    std::cout << "pi: " << pi<float> << "\n";
}