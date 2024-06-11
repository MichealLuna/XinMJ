#ifndef XINMJ_BASE_EXCEPTION_H
#define XINMJ_BASE_EXCEPTION_H

#include <exception>
#include <string>

namespace xin
{
    
    class Exception : std::exception
    {
    public:
        explicit Exception(const char *e);
        explicit Exception(const std::string &e);
        virtual const char *what() const throw();
        const char *stackTrace() const throw();

    private:
        void fillStackTrack();

        std::string message_;
        std::string stack_;
    };

} // namespace xin

#endif //! XINMJ_BASE_EXCEPTION_H