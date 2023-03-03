#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <deque>
#include <memory>

void foo(int x = 42)
{ }

TEST_CASE("default value for arg")
{
    foo();
    foo(665);
}

template <typename T, typename TContainer = std::deque<T>>
class Stack
{
    TContainer items_;

public:
    Stack() = default;

    bool empty() const
    {
        return items_.empty();
    }

    size_t size() const
    {
        return items_.size();
    }

    // void push(const T& item)
    // {
    //     items_.push_back(item);
    // }

    // void push(T&& item) // T&& - always r-value ref
    // {
    //     items_.push_back(std::move(item));
    // }

    template <typename U>
    void push(U&& item /* universal reference*/) // U is function template parameter
    {
        items_.push_back(std::forward<U>(item));
    }

    const T& top() const
    {
        return items_.back();
    }

    T& top()
    {
        return items_.back();
    }

    void pop()
    {
        items_.pop_back();
    }
};

namespace ver_2_0
{
    template <
        typename T, 
        template <typename, typename> class TContainer, // template parameter
        typename TAllocator = std::allocator<T>
    >
    class Stack
    {
        TContainer<T, TAllocator> items_;

    public:
        Stack() = default;

        bool empty() const
        {
            return items_.empty();
        }

        size_t size() const
        {
            return items_.size();
        }

        // void push(const T& item)
        // {
        //     items_.push_back(item);
        // }

        // void push(T&& item) // T&& - always r-value ref
        // {
        //     items_.push_back(std::move(item));
        // }

        template <typename U>
        void push(U&& item /* universal reference*/) // U is function template parameter
        {
            items_.push_back(std::forward<U>(item));
        }

        const T& top() const
        {
            return items_.back();
        }

        T& top()
        {
            return items_.back();
        }

        void pop()
        {
            items_.pop_back();
        }
    };
} // namespace ver_2_0

TEST_CASE("After construction", "[stack,constructors]")
{
    Stack<int> s;

    SECTION("is empty")
    {
        REQUIRE(s.empty());
    }

    SECTION("size is zero")
    {
        REQUIRE(s.size() == 0);
    }
}

TEST_CASE("Pushing an item", "[stack,push]")
{
    Stack<int> s;

    SECTION("is no longer empty - pushing rvalue")
    {
        s.push(1);

        REQUIRE(!s.empty());
    }

    SECTION("is no longer empty - pushing lvalue")
    {
        int x = 10;
        s.push(x);

        REQUIRE(!s.empty());
    }

    SECTION("size is increased")
    {
        auto size_before = s.size();

        s.push(1);

        REQUIRE(s.size() - size_before == 1);
    }

    SECTION("recently pushed item is on a top")
    {
        s.push(4);

        REQUIRE(s.top() == 4);
    }
}

template <typename T>
std::vector<T> pop_all(Stack<T>& s)
{
    std::vector<T> values(s.size());

    for (auto& item : values)
    {
        item = std::move(s.top());
        s.pop();
    }

    return values;
}

TEST_CASE("Popping an item", "[stack,pop]")
{
    ver_2_0::Stack<int, std::vector> s;

    s.push(1);
    s.push(4);

    int item;

    SECTION("assignes an item from a top to an argument passed by ref")
    {
        item = s.top();
        s.pop();

        REQUIRE(item == 4);
    }

    SECTION("size is decreased")
    {
        size_t size_before = s.size();

        item = s.top();
        s.pop();

        REQUIRE(size_before - s.size() == 1);
    }

    SECTION("LIFO order")
    {
        int a, b;

        a = s.top();
        s.pop();

        b = s.top();
        s.pop();

        REQUIRE(a == 4);
        REQUIRE(b == 1);
    }
}

TEST_CASE("Move semantics", "[stack,push,pop,move]")
{
    using namespace std::literals;

    SECTION("stores move-only objects")
    {
        auto txt1 = std::make_unique<std::string>("test1");

        Stack<std::unique_ptr<std::string>> s;

        s.push(move(txt1));
        s.push(std::make_unique<std::string>("test2"));

        std::unique_ptr<std::string> value;

        value = std::move(s.top());
        s.pop();
        REQUIRE(*value == "test2"s);

        value = std::move(s.top());
        s.pop();
        REQUIRE(*value == "test1"s);
    }

    SECTION("move constructor", "[stack,move]")
    {
        Stack<std::unique_ptr<std::string>> s;

        s.push(std::make_unique<std::string>("txt1"));
        s.push(std::make_unique<std::string>("txt2"));
        s.push(std::make_unique<std::string>("txt3"));

        auto moved_s = std::move(s);

        auto values = pop_all(moved_s);

        auto expected = {"txt3", "txt2", "txt1"};
        REQUIRE(std::equal(values.begin(), values.end(), expected.begin(), [](const auto& a, const auto& b) { return *a == b; }));
    }

    SECTION("move assignment", "[stack,move]")
    {
        Stack<std::unique_ptr<std::string>> s;

        s.push(std::make_unique<std::string>("txt1"));
        s.push(std::make_unique<std::string>("txt2"));
        s.push(std::make_unique<std::string>("txt3"));

        Stack<std::unique_ptr<std::string>> target;
        target.push(std::make_unique<std::string>("x"));

        target = std::move(s);

        REQUIRE(target.size() == 3);

        auto values = pop_all(target);

        auto expected = {"txt3", "txt2", "txt1"};
        REQUIRE(std::equal(values.begin(), values.end(), expected.begin(), [](const auto& a, const auto& b) { return *a == b; }));
    }
}