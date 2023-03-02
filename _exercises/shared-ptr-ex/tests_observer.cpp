#include <cassert>
#include <cstdlib>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <memory>
#include <catch2/catch_test_macros.hpp>

class Observer
{
public:
    virtual void update(const std::string& event_args) = 0;
    virtual ~Observer() = default;
};

class Subject
{
private:
    using ObserverWPtr = std::weak_ptr<Observer>;
    using ObserverWPtrComparer = std::owner_less<ObserverWPtr>;
 
    int state_;
    std::set<ObserverWPtr, ObserverWPtrComparer> observers_;

public:
    Subject() : state_(0)
    {
    }

    void register_observer(ObserverWPtr observer)
    {
        observers_.insert(observer);
    }

    void unregister_observer(ObserverWPtr observer)
    {
        observers_.erase(observer);
    }

    void set_state(int new_state)
    {
        if (state_ != new_state)
        {
            state_ = new_state;
            notify("Changed state on: " + std::to_string(state_));
        }
    }

protected:
    void notify(const std::string& event_args)
    {
        for (auto it = observers_.begin(); it != observers_.end();)
        {
            ObserverWPtr observer = *it;

            std::shared_ptr<Observer> living_observer = observer.lock();
            if (living_observer)
            {
                living_observer->update(event_args);
                ++it;
            }
            else
            {
                it = observers_.erase(it);
            }
        }
    }
};

class ConcreteObserver1 : public Observer, public std::enable_shared_from_this<ConcreteObserver1>
{
public:
    virtual void update(const std::string& event)
    {
        std::cout << "ConcreteObserver1: " << event << std::endl;
    }

    void register_me_as_observer(Subject& s)
    {
        s.register_observer(this->shared_from_this());
    }

    void do_stuff()
    {
        std::cout << "ConcreteObserver1::do_stuff()\n";
    }
};

class ConcreteObserver2 : public Observer
{
public:
    virtual void update(const std::string& event)
    {
        std::cout << "ConcreteObserver2: " << event << std::endl;
    }
};

TEST_CASE("using observer pattern")
{
    using namespace std;

    Subject s;

    std::shared_ptr<Observer> o1 = std::make_shared<ConcreteObserver1>();
    
    // dynamic_cast
    std::shared_ptr<ConcreteObserver1> co1 = std::dynamic_pointer_cast<ConcreteObserver1>(o1);
    if (co1)
        co1->do_stuff();

    s.register_observer(o1);

    {
        auto o2 = std::make_shared<ConcreteObserver2>();
        s.register_observer(o2);

        s.set_state(1);

        o2.reset();

        cout << "End of scope." << endl;
    }

    s.set_state(2); // call of update() on deleted object
}
