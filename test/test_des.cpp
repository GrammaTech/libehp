#include <exception>
#include <iostream>

class myException1 : public std::exception
{
public:
    virtual const char* what() const throw()
    {
        return "threw myException1";
    }
};

class myException2 : public std::exception
{
public:
    virtual const char* what() const throw()
    {
        return "threw myException2";
    }
};

class myException3 : public std::exception
{
public:
    virtual const char* what() const throw()
    {
        return "threw myException3";
    }
};

/*
    void func(int bound) throw (myException)

produces:

    warning: dynamic exception specifications are deprecated in C++11 [-Wdeprecated]
*/

template <class T>
class MyThing
{
public:
    myException1 exception1;
    myException2 exception2;
    myException3 exception3;

    MyThing(T arg, int bound) : bound(bound), arg(arg)
    {
    }

    T& operator[](int index) throw(myException1, myException2, myException3)
    {
        // Throwing exception2 and exception3 here is non-sense,
        // but this example is to demonstrate that libehp handles
        // the Dynamic Exception Specification (DES) feature correctly.
        if(index > bound)
        {
            throw exception1;
        }
        else if(index < bound && index > bound-7)
        {
            throw exception2;
        }
        else if(index < bound-7)
        {
            throw exception3;
        }

        std::cout << "no thrown\n";
        return arg;
    }

private:
    T arg;
    int bound;
};

int main()
{
    MyThing<int> thing = MyThing<int>(5, 10);

    try
    {
        thing[20];
    }
    catch(myException1& e)
    {
        std::cout << e.what() << std::endl;
    }

    try
    {
        thing[7];
    }
    catch(myException2& e)
    {
        std::cout << e.what() << std::endl;
    }

    try
    {
        thing[2];
    }
    catch(myException3& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}

