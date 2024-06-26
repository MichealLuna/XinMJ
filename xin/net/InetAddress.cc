#include <xin/net/InetAddress.h>
#include <xin/net/Endian.h>
#include <xin/net/SocketsOps.h>

#include <netdb.h>
#include <strings.h>
#include <netinet/in.h>

#pragma GCC diagnostic ignored "-Word-style-cast"
static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;
#pragma GCC diagnostic error "-Word-style-cast"


using namespace xin;
using namespace net;

//     /* Structure describing an Internet socket address.  */
//     struct sockaddr_in {
//         sa_family_t    sin_family; /* address family: AF_INET */
//         uint16_t       sin_port;   /* port in network byte order */
//         struct in_addr sin_addr;   /* internet address */
//     };

//     /* Internet address. */
//     typedef uint32_t in_addr_t;
//     struct in_addr {
//         in_addr_t       s_addr;     /* address in network byte order */
//     };

//     struct sockaddr_in6 {
//         sa_family_t     sin6_family;   /* address family: AF_INET6 */
//         uint16_t        sin6_port;     /* port in network byte order */
//         uint32_t        sin6_flowinfo; /* IPv6 flow information */
//         struct in6_addr sin6_addr;     /* IPv6 address */
//         uint32_t        sin6_scope_id; /* IPv6 scope-id */
//     };

#if 4 <= __GNUC__
#define offsetof(type,member)       __builtin_offsetof(type,member)
#else
#define offsetof(sst,mem)       (uw)((&((typeof(sst)*)0)->mem))
#endif

static_assert(sizeof(InetAddress) == sizeof(struct sockaddr_in6),"inet address size wrong!");
static_assert(offsetof(sockaddr_in,sin_family) == 0,"offsetof sin_family in sockaddr_in wrong!");
static_assert(offsetof(sockaddr_in6,sin6_family) == 0,"offsetof sin6_family in sockaddr_in6 wrong!");
static_assert(offsetof(sockaddr_in,sin_port) == 2,"offsetof sin_port in sockaddr_in wrong!");
static_assert(offsetof(sockaddr_in6,sin6_port) == 2,"offsetof sin6_port in sockaddr_in6 wrong!");

InetAddress::InetAddress(uint16_t port,bool loopbackOnly,bool ipv6)
{
    static_assert(offsetof(InetAddress,addr_) == 0,"offsetof addr_ in InetAddress is not 0");
    static_assert(offsetof(InetAddress,addr6_) == 0,"offsetof addr6_ in InetAddress is not 0");
    if(ipv6)
    {
        bzero(&addr6_,sizeof(addr6_));
        addr6_.sin6_family = AF_INET6;
        in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
        addr6_.sin6_addr = ip;
        addr6_.sin6_port = sockets::hostToNetwork16(port);
    }
    else
    {
        bzero(&addr_,sizeof(addr_));
        addr_.sin_family = AF_INET;
        in_addr_t ip = loopbackOnly ? kInaddrLoopback : kInaddrAny;
        addr_.sin_addr.s_addr = sockets::hostToNetwork32(ip);
        addr_.sin_port = sockets::hostToNetwork16(port);
    }
}

InetAddress::InetAddress(StringArg ip,uint16_t port,bool ipv6)
{
    if(ipv6)
    {
        bzero(&addr6_,sizeof(addr6_));
        sockets::fromIpPort(ip.c_str(),port,&addr6_);
    }
    else
    {
        bzero(&addr_,sizeof(addr_));
        sockets::fromIpPort(ip.c_str(),port,&addr_);
    }
}

string InetAddress::toIp() const
{
    char buf[64] = "";
    sockets::toIp(buf,sizeof(buf),getSockAddr());
    return buf;
}

string InetAddress::toIpPort() const
{
    char buf[64] = "";
    sockets::toIpPort(buf,sizeof(buf),getSockAddr());
    return buf;
}

uint32_t InetAddress::ipNetEndian() const
{
    assert(family() == AF_INET);
    return addr_.sin_addr.s_addr;
}

uint16_t InetAddress::toPort() const
{
    return sockets::network16ToHost(portNetEndian());
}

static __thread char t_resolveBuffer[64 * 1024];

bool InetAddress::resolve(StringArg hostname,InetAddress* out)
{
    assert(out != NULL);
    struct hostent hent;
    struct hostent* he = NULL;
    int herrno = 0;
    bzero(&hent,sizeof(hent));

    int ret = gethostbyname_r(hostname.c_str(),&hent,t_resolveBuffer,sizeof(t_resolveBuffer),&he,&herrno);
    if(ret == 0 && he != NULL)
    {
        assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
        out->addr_.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
        return true;
    }
    else
    {
        if(ret)
        {
            fprintf(stderr,"InetAddress::resolve");
        }
        return false;
    }
}