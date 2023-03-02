#include "utils.hpp"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

using Utils::Gadget;

// https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-resource

namespace LegacyCode
{
    // forward declarations
    Gadget* get_gadget(const std::string& name);
    void use(Gadget* g);
    void use_gadget(Gadget* g);

    // definitions
    Gadget* get_gadget(const std::string& name)
    {
        static int id = 665;
        return new Gadget(++id, name);
    }

    void use(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";

        delete g;
    }

    void use_gadget(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }
} // namespace LegacyCode

TEST_CASE("legacy hell with dynamic memory")
{
    using namespace LegacyCode;

    {
        Gadget* g = get_gadget("ipad");

        use_gadget(g);
    } // memory leak

    {
        use_gadget(get_gadget("ipad"));
    } // memory leak

    {
        Gadget* g = new Gadget(13, "ipad");
        

        use(g);
        
        //use_gadget(g);  // UB!!! - use after delete
        //std::cout << g->name() << std::endl; // UB!!! - use after delete
    }

    {
        Gadget* g = get_gadget("ipad");
        

        use(g);

        //delete g; // UB!!! - second delete
    }
}

namespace ModernCpp
{
    std::unique_ptr<Gadget> get_gadget(const std::string& name);

    std::unique_ptr<Gadget> get_gadget(const std::string& name)
    {
        static int id = 665;
        
        return std::make_unique<Gadget>(++id, name);
    }

    void use(std::unique_ptr<Gadget> g) // sink
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }

    void use_gadget(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }
}

TEST_CASE("memory management in modern C++")
{
    using namespace ModernCpp;

    {
        std::unique_ptr<Gadget> g = get_gadget("ipad");

        use_gadget(g.get());
    }

    {
        use(get_gadget("ipad"));
    } 

    {
        auto g = std::make_unique<Gadget>(13, "ipad");
        

        use(std::move(g));

        g = get_gadget("smartwatch");

        if (g)
            std::cout << "Using " << g->name() << "\n";
    }

    {
        auto g = std::make_unique<Gadget>(13, "ipad");
        
        use(std::move(g));
    }
}

class Owner
{
    std::string name_;
    std::unique_ptr<Gadget> gadget_;
public:
    Owner(std::string name, std::unique_ptr<Gadget> gadget)
        : name_(std::move(name)), gadget_(std::move(gadget))
    {}

    void use() const
    {
        if (gadget_)
            std::cout << name_  << " is using " << gadget_->name() << "\n";
    }
};

TEST_CASE("owner of gadgets")
{
    Owner jk{"Jan Kowalski", ModernCpp::get_gadget("ipad")};
    jk.use();

    auto g = ModernCpp::get_gadget("smartwatch");
    Owner an{"Adam Nowak", std::move(g)};
    an.use();
}

TEST_CASE("unique_ptr<T[]>")
{
    //Gadget* tab = new Gadget[10];
    std::unique_ptr<Gadget[]> tab(new Gadget[10]);

    tab[4].name();
}