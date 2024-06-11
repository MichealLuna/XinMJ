#include <xin/base/Exception.h>

#include <execinfo.h>
#include <stdlib.h>

using namespace xin;

Exception::Exception(const char* e)
  : message_(e)
{
    fillStackTrace();
}

Exception::Exception(const std::string &e)
  : message_(e)
{
    fillStackTrace();
}

Exception::~Exception() throw()
{
}

const char * Exception::what() const throw()
{
    return message_.c_str();
}

const char * Exception::stackTrace() const throw()
{
    return stack_.c_str();
}

void Exception::fillStackTrace()
{
    const int sz = 256;
    void* buf[sz];
    int nbufs = ::backtrace(buf,sz);
    char** strs = ::backtrace_symbols(buf,nbufs);
    if(strs)
    {
        for(int i = 0; i < nbufs; ++i)
        {
            stack_.append(strs[i]);
            stack_.push_back('\n');
        }
        free(strs);
    }
}