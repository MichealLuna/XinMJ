#ifndef XINMJ_BASE_NONCOPYABLE_H
#define XINMJ_BASE_NONCOPYABLE_H

namespace xin
{
    /*
    *   it can be moved but can't be copied.
    */
    class noncopyable{
        private:
        noncopyable(const noncopyable&) = delete;
        void operator=(const noncopyable&) = delete;

        protected:
        noncopyable() = default;
        ~noncopyable() = default;
    };
} // namespace xin
#endif//!XINMJ_BASE_NONCOPYABLE_H