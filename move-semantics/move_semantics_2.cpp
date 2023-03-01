#include "gadget.hpp"

#include <catch2/catch_test_macros.hpp>
#include <memory>

////////////////////////////////////////////////
// simplified implementation of unique_ptr - only moveable type

namespace Explain
{
    template <typename T>
    class unique_ptr
    {
    public:
        unique_ptr(T* ptr = nullptr)
            : ptr_{ptr}
        {
        }

        unique_ptr(const unique_ptr&) = delete;            // copy constructor is deleted
        unique_ptr& operator=(const unique_ptr&) = delete; // copy assignment operator is deleted

        // move constructor
        unique_ptr(unique_ptr&& source)
            : ptr_{source.ptr_}
        {
            source.ptr_ = nullptr;
        }

        // move assignment operator
        unique_ptr& operator=(unique_ptr&& source)
        {
            if (this != &source) 
            {
                delete ptr_;

                ptr_ = source.ptr_;
                source.ptr_ = nullptr;
            }

            return *this;
        }

        ~unique_ptr()
        {
            delete ptr_;
        }

        operator bool() const
        {
            return ptr_ != nullptr;
        }

        T& operator*() const // (*smart_ptr).use();
        {
            return *ptr_;
        }

        T* operator->() const
        {
            return ptr_;
        }

        T* get() const
        {
            return ptr_;
        }

    private:
        T* ptr_;
    };
} // namespace Explain

using namespace Helpers;

void may_throw(int x)
{
    if (x == 13)
        throw std::runtime_error("Error#13");
}

TEST_CASE("why smart pointers")
{
    SECTION("before C++11")
    {
        Gadget* ptr_g = new Gadget(1, "ipad");

        ptr_g->use();
        (*ptr_g).use();

        may_throw(15);

        delete ptr_g;
    }
}

TEST_CASE("move semantics - unique_ptr")
{
    Explain::unique_ptr<Gadget> ptr_null;

    Explain::unique_ptr<Gadget> ptr_g{new Gadget(2, "ipod")};

    if (ptr_g)
    {
        ptr_g->use();
        (*ptr_g).use();
    }

    may_throw(15);

    // Explain::unique_ptr<Gadget> other_ptr = ptr_g; // ERROR - unique_ptr is not copyable

    // gadget is moved from ptr_g to other_ptr
    Explain::unique_ptr<Gadget> other_ptr(std::move(ptr_g)); // move constructor is called

    if (other_ptr)
        other_ptr->use();

    REQUIRE(ptr_g.get() == nullptr);

    Explain::unique_ptr<Gadget> ptr_target(new Gadget(3, "smartwatch"));

    if (ptr_target)
        ptr_target->use();

    ptr_target = std::move(other_ptr); // move assignment operator

    REQUIRE(other_ptr.get() == nullptr);

    ptr_target->use();

    ptr_target = std::move(ptr_target); // self-assignment
}

std::unique_ptr<Gadget> create_gadget()
{
    static int id_gen = 0;

    const int id = ++id_gen;
    return std::unique_ptr<Gadget>(new Gadget(id, "Gadget#" + std::to_string(id)));
}

TEST_CASE("using unique_ptr")
{
    std::unique_ptr<Gadget> ptr_1 = create_gadget();

    if (ptr_1)
        ptr_1->use();

    std::vector<std::unique_ptr<Gadget>> gadgets;
    gadgets.push_back(std::move(ptr_1));
    gadgets.push_back(create_gadget());
    gadgets.push_back(create_gadget());
    gadgets.push_back(create_gadget());

    for(const auto& g : gadgets)
    {
        if (g)
            g->use();
    }
}