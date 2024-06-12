#include <xin/base/Exception.h>
#include <iostream>

using namespace xin;

class myException : public Exception
{
    public:
        explicit myException(const char* e)
         : Exception(e)
        {
        }

        const char* what() const throw() override
        {
            return "myException";
        }

        ~myException() throw() override
        {
            
        }
};

class Testing
{
public:
    void test()
    {
        throw myException("Testing exception class...");
    }
};

void test()
{
    Testing t;
    t.test();
}

int main()
{
    try
    {
        test();
    }
    catch(const xin::Exception& e)
    {
        std::cerr << e.what() << "\n";
        std::cerr << e.stackTrace() << "\n";
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    catch(...)
    {
        throw;
    }
    return 0;
}