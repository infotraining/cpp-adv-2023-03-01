#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

using namespace std;

class Gadget
{
public:
    Gadget(int id = 0)
        : id_{id}
    {
        std::cout << "Constructing Gadget(" << id_ << ")\n";
    }

    Gadget(const Gadget&) = delete;
    Gadget& operator=(const Gadget&) = delete;

    ~Gadget()
    {
        std::cout << "Destroying ~Gadget(" << id_ << ")\n";
    }

    int id() const
    {
        return id_;
    }

    void set_id(int id)
    {
        id_ = id;
    }

    void use()
    {
        std::cout << "Using a gadget with id: " << id() << '\n';
    }

    void unsafe()
    {
        std::cout << "Using a gadget with id: " << id() << " - Ups... It crashed..." << std::endl;
        throw std::runtime_error("ERROR");
    }

private:
    int id_;
};

namespace LegacyCode
{
    Gadget* create_many_gadgets(unsigned int size)
    {
        Gadget* many_gadgets = new Gadget[size];

        for (unsigned int i = 0; i < size; ++i)
            many_gadgets[i].set_id(i);

        return many_gadgets;
    }
}

void reset_value(Gadget& g, int n)
{
    // some logic

    g.set_id(n);
    cout << "New id for Gadget: " << g.id() << endl;
}

//////////////////////////////////////////////
// TODO - modernize the code below

std::unique_ptr<Gadget> create_gadget(int arg)
{
    return std::make_unique<Gadget>(arg);
}

class Player
{
    std::unique_ptr<Gadget> gadget_; // owning pointer
    std::ostream* logger_; // non-owning pointer



public:
    Player(std::unique_ptr<Gadget> g, std::ostream* logger = nullptr)
        : gadget_(std::move(g))
        , logger_(logger)
    {
        if (!g)
            throw std::invalid_argument("Gadget can not be null");
    }

    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    Player(Player&&) = default;
    Player& operator=(Player&&) = default;

    ~Player()
    {
        if (logger_)
            *logger_ << "Destroying a gadget: " << gadget_->id() << std::endl;
    }

    void play()
    {
        if (logger_)
            *logger_ << "Player is using a gadget: " << gadget_->id() << std::endl;

        gadget_->use();
    }
};

void unsafe1()
{
    std::unique_ptr<Gadget> ptr_gdgt = create_gadget(4);

    /* kod korzystajacy z ptr_gdgt */

    reset_value(*ptr_gdgt, 5);

    ptr_gdgt->unsafe();
}

void unsafe2()
{
    int size = 10;

    std::unique_ptr<Gadget[]> buffer(LegacyCode::create_many_gadgets(size));

    for (int i = 0; i < size; ++i)
        buffer[0].unsafe();
}

void unsafe3() // TODO: modernize using smart pointers
{
    vector<std::unique_ptr<Gadget>> my_gadgets;

    my_gadgets.push_back(create_gadget(87));
    my_gadgets.push_back(create_gadget(12));
    my_gadgets.push_back(std::make_unique <Gadget>(98));

    int value_generator = 0;
    for (const auto& ptr_g : my_gadgets)
    {
        cout << "Gadget's old id: " << ptr_g->id() << endl;
        reset_value(*ptr_g, ++value_generator);
    }

    my_gadgets[0] = nullptr;

    Player p(std::move(my_gadgets.back()));
    my_gadgets.pop_back();
    p.play();

    my_gadgets[1]->unsafe();
}

int main() try
{
    try
    {
        unsafe1();
        unsafe2();
        unsafe3();
    }
    catch (const exception& e)
    {
        cout << "Exception caught: " << e.what() << endl;
    }
}
catch (const exception& e)
{
    cout << "Exception caught: " << e.what() << endl;
}