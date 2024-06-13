#ifndef XINMJ_BASE_TYPES_H
#define XINMJ_BASE_TYPES_H

#include <stdint.h>

#ifdef XIN_STD_STRING
#include <string>
#else //!XIN_STD_STRING
#include <ext/vstring.h>
#include <ext/vstring_fwd.h>
#endif

#ifndef NDEBUG
#include<assert.h>
#endif// debug mode.

#include <xin/base/noncopyable.h>

namespace xin
{

#ifdef XIN_STD_STRING
using std::string
#else
typedef __gnu_cxx::__sso_string string;
#endif

template<class To,class From>
inline To implicit_cast(From const& f)
{
    return f;
}

/*
*   @From: dirved class
#   @To  : base class
*/
template<typename To,typename From>
inline To down_cast(From* f)
{
    //only in compile-time
    if(false)
    {
        implicit_cast<From*,To>(0);
    }
#if !defined(NDEBUG) || !defined(GOOGLE_PROTOBUF_NO_RTTI)
    assert(f == NULL || dynamic_cast<To>(f) != NULL);
#endif
    return static_cast<To>(f);
}

} // namespace xin

#endif//!XINMJ_BASE_TYPES_H