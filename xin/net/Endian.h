#ifndef XINMJ_NET_ENDIAN_H
#define XINMJ_NET_ENDIAN_H

#include <stdint.h>
#include <endian.h>

namespace xin
{
namespace net
{
namespace sockets
{

#if defined(__clang__) || __GNUC_MINOR__ >= 6
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wold-style-cast"
inline uint64_t hostToNetwork64(uint64_t host64)
{
    return htobe64(host64);
}

inline uint32_t hostToNetwork32(uint32_t host32)
{
    return htobe32(host32);
}

inline uint16_t hostToNetwork16(uint16_t host16)
{
    return htobe16(host16);
}

inline uint64_t network64ToHost(uint64_t net64)
{
    return be64toh(net64);
}

inline uint32_t network32ToHost(uint32_t net32)
{
    return be32toh(net32);
}

inline uint16_t network16ToHost(uint16_t net16)
{
    return be16toh(net16);
}

#if defined(__clang__) || __GNUC_MINOR__ >= 6
#pragma GCC diagnostic pop
#else
#pragma diagnostic warning "-Wconversion"
#pragma diagnostic warning "-Wold-style-cast"
#endif

}//namespace sockets
}//namespace net
}//namespace xin
    
#endif//!XINMJ_NET_ENDIAN_H