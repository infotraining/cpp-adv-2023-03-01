#include <catch2/catch_test_macros.hpp>
#include <iostream>

using namespace std::literals;

std::string full_name(const std::string& first_name, const std::string& last_name)
{
    return first_name + " " + last_name;
}

TEST_CASE("reference binding")
{
    std::string name = "jan";

    SECTION("C++98")
    {
        std::string& ref_name = name;

        const std::string& ref_full_name = full_name(name, "kowalski");

        // ref_full_name[0] = 'p'; // ERROR
    }

    SECTION("C++11")
    {
        std::string&& rvalue_ref_full_name = full_name(name, "kowalski");

        rvalue_ref_full_name[0] = 'p';

        std::cout << rvalue_ref_full_name << "\n";

        // std::string&& rvalue_ref_name = name; // ERROR - cannot bind rvalue reference of type 'std::string&&' to lvalue of type 'std::string'
    }
}

class VectorString
{
public:
    std::vector<std::string> items;

    void push_back(const std::string& str)
    {
        std::cout << "push_back makes copy of str to the destination\n";
        items.push_back(str);
    }

    void push_back(std::string&& str)
    {
        std::cout << "push_back moves str to the destination\n";
        items.push_back(std::move(str));
    }
};

TEST_CASE("push_back in vector")
{
    std::string name = "jan";

    VectorString vec;

    vec.push_back(name); // lvalue -> push_back(const string&)

    vec.push_back(full_name("jan", "kowalski")); // rvalue -> push_back(string&&)

    REQUIRE(vec.items == std::vector<std::string>{"jan", "jan kowalski"});
}

TEST_CASE("objects & move")
{
    SECTION("classes")
    {
        std::vector<int> vec = {1, 2, 3};
        std::vector<int> backup = vec;            // copy
        std::vector<int> target = std::move(vec); // move

        REQUIRE(target == std::vector{1, 2, 3});
        REQUIRE(vec.size() == 0); // better to avoid

        std::string str = "text";
        std::string backup_str = str;            // copy
        std::string target_str = std::move(str); // move
        REQUIRE(str.size() == 0);                // better to avoid
    }

    SECTION("primitive types -  std::move makes copy")
    {
        int x = 10;
        int backup_x = x;            // copy
        int target_x = std::move(x); // copy

        int* ptr = &x;
        int* ptr_backup = ptr;           // copy
        int* ptr_target = std::move(ptr); // copy
    }
}
