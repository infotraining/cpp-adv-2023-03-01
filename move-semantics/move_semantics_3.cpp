#include "helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <utility>

////////////////////////////////////////////////////////////////////////////
// Data - class with copy & move semantics (user provided implementation)

using namespace Helpers;

class Data
{
    std::string name_;
    int* data_;
    size_t size_;

public:
    using iterator = int*;
    using const_iterator = const int*;

    Data(std::string name, std::initializer_list<int> list)
        : name_{std::move(name)}
        , size_{list.size()}
    {
        data_ = new int[list.size()];
        std::copy(list.begin(), list.end(), data_);

        std::cout << "Data(" << name_ << ")\n";
    }

    Data(const Data& other) // copy constructor
        : name_(other.name_)
        , size_(other.size_)
    {
        std::cout << "Data(" << name_ << ": cc)\n";
        data_ = new int[size_];
        std::copy(other.begin(), other.end(), data_);
    }

    Data& operator=(const Data& other) // copy assignment
    {
        Data temp(other);
        swap(temp);

        std::cout << "Data=(" << name_ << ": cc)\n";

        return *this;
    }

    // move semantics
    Data(Data&& source)
        : name_(std::move(source.name_))
        , data_(std::exchange(source.data_, nullptr))
        , size_(std::exchange(source.size_, 0))
    {
        std::cout << "Data(" << name_ << ": mv)\n";
    }

    Data& operator=(Data&& source)
    {
        if (this != &source)
        {
            delete[] data_;
            name_ = std::move(source.name_);
            data_ = std::exchange(source.data_);
            size_ = std::exchange(source.size_);
        }

        std::cout << "Data=(" << name_ << ": mv)\n";

        return *this;
    }

    ~Data()
    {
        delete[] data_;
    }

    void swap(Data& other)
    {
        name_.swap(other.name_);
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
    }

    iterator begin()
    {
        return data_;
    }

    iterator end()
    {
        return data_ + size_;
    }

    const_iterator begin() const
    {
        return data_;
    }

    const_iterator end() const
    {
        return data_ + size_;
    }
};

Data create_data_set()
{
    Data ds{"data-set-one", {54, 6, 34, 235, 64356, 235, 23}};

    return ds;
}

TEST_CASE("Data & move semantics")
{
    Data ds1{"ds1", {1, 2, 3, 4, 5}};

    Data backup = ds1; // copy
    print("backup", backup);

    Data target = std::move(ds1);
    print("target", target);
}

TEST_CASE("copy - how it works")
{
    std::vector<int> vec = {1, 2, 3, 4};
    std::vector<int> backup1(vec.size());

    std::copy(vec.begin(), vec.end(), backup1.begin());

    int* tab = new int[vec.size()]; // dynamic array - allocate memory new[]
    std::copy(vec.begin(), vec.end(), tab);

    delete[] tab; // free memory
}