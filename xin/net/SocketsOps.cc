#include <xin/net/SocketsOps.h>

#include <xin/base/Types.h>
#include <xin/net/Endian.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace xin;
using namespace xin::net;

namespace
{

typedef struct sockaddr SA;

#if VALGRIND || defined (NO_ACCEPT4)
void setNonBlockAndCloseOnExec(int sockfd)
{
    int flags = ::fcntl(sockfd,F_GETFL,0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(sockfd,F_SETFL,flags);

    flags = ::fcntl(sockfd,F_GETFD,0);
    flags |= O_CLOEXEC;
    ret = ::fcntl(sockfd,F_SETFD,flags);
    void(ret);
}
#endif
}

int sockets::createNonBlockingOrDie(sa_family_t family)
{
#if VALGRIND
    int sockfd = ::socket(family,SOCK_STREAM,IPPROTO_TCP);
    if(sockfd < 0)
    {
        /*... other,eg: log*/
        abort();
    }
    setNonBlockAndCloseOnExec(sockfd);
#else
    int sockfd = ::socket(family,SOCK_STREAM | SOCK_CLOEXEC | SOCK_CLOEXEC,IPPROTO_TCP);
    if(sockfd < 0)
    {
        /*... other,eg: log*/
        abort();
    }
#endif
    return sockfd;
}


int sockets::connect(int sockfd,const struct sockaddr* addr)
{
    return ::connect(sockfd,addr,static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
}

void sockets::bindOrDie(int sockfd,const struct sockaddr* addr)
{
    int ret = ::bind(sockfd,addr,static_cast<socklen_t>(sizeof(sockaddr_in6)));
    if(ret < 0)
    {
        /*or log*/
        abort();
    }
}

void sockets::listenOrDie(int sockfd)
{
    int ret = ::listen(sockfd,SOMAXCONN);
    if(ret < 0)
    {
        /*...*/
        abort();
    }
}

int sockets::accept(int sockfd,struct sockaddr_in6* addr)
{
    socklen_t addrlen = static_cast<socklen_t>(sizeof *addr);
#if VALGRIND || defined(NO_ACCEPT4)
    int connfd = ::accept(sockfd,sockaddr_cast(addr),&addrlen);
    setNonBlockAndCloseOnExec(connfd);
#else
    int connfd = ::accept4(sockfd,sockaddr_cast(addr),&addrlen,
                        SOCK_CLOEXEC | SOCK_NONBLOCK);
#endif
    if(connfd < 0)
    {
        int savedErrno = errno;
        fprintf(stderr,"sockets::accept");
        switch(savedErrno)
        {
            case EAGAIN:
            case ECONNABORTED:
            case EINTR:
            case EPROTO:
            case EPERM:
            case EMFILE:
                errno = savedErrno;
                break;
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENFILE:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
                /*log...*/
                fprintf(stderr,"unexcepted error of ::accept: %d",savedErrno);
                break;
            default:
                fprintf(stderr,"unknown  error of ::accept: %d",savedErrno);
                break;
        }
    }
    return connfd;
}

ssize_t sockets::read(int sockfd,void* buf,size_t count)
{
    return ::read(sockfd,buf,count);
}

ssize_t sockets::readv(int sockfd,const struct iovec* iov,int iovcnt)
{
    /*...*/
    return xin::net::sockets::readv(sockfd,iov,iovcnt);
}

ssize_t sockets::write(int sockfd,const void* buf,size_t count)
{
    return ::write(sockfd,buf,count);
}

void sockets::close(int sockfd)
{
    if(::close(sockfd) < 0)
    {
        /*log*/
    }
}

void sockets::shutdownWrite(int sockfd)
{
    if(::shutdown(sockfd,SHUT_WR) < 0)
    {
        /*log*/
    }
}


void sockets::toIpPort(char* buf,size_t size,const struct sockaddr* addr)
{
    toIp(buf,size,addr);
    size_t end = ::strlen(buf);
    const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
    uint16_t port = sockets::network16ToHost(addr4->sin_port);
    assert(size > end);
    snprintf(buf + end,size - end,":%u",port);
}


void sockets::toIp(char* buf,size_t size,const struct sockaddr* addr)
{
    if(addr->sa_family == AF_INET)
    {
        assert(size >= INET_ADDRSTRLEN);
        const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
        ::inet_ntop(AF_INET,&addr4->sin_addr,buf,static_cast<socklen_t>(size));
    }
    else if(addr->sa_family == AF_INET6)
    {
        assert(size >= INET6_ADDRSTRLEN);
        const struct sockaddr_in6* addr6 = sockaddr_in6_cast(addr);
        ::inet_ntop(AF_INET6,&addr6->sin6_addr,buf,static_cast<socklen_t>(size));
    }
}


void sockets::fromIpPort(const char* ip,uint16_t port,struct sockaddr_in* addr)
{
    addr->sin_family = AF_INET;
    addr->sin_port = hostToNetwork16(port);
    if(::inet_pton(AF_INET,ip,&addr->sin_addr) <= 0)
    {
        fprintf(stderr,"sockets::fromIpPort");
    }
}

void sockets::fromIpPort(const char* ip,uint16_t port,struct sockaddr_in6* addr)
{
    addr->sin6_family = AF_INET6;
    addr->sin6_port = hostToNetwork16(port);
    if(::inet_pton(AF_INET,ip,&addr->sin6_addr) <= 0)
    {
        fprintf(stderr,"sockets::fromIpPort");
    }
}


int sockets::getSocketError(int sockfd)
{
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof(optval));

    if(::getsockopt(sockfd,SOL_SOCKET,SO_ERROR,&optval,&optlen) < 0)
    {
        return errno;
    }
    else
    {
        return optval;
    }
}


const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in* addr)
{
    return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
}

const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in6* addr)
{
    return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
}

struct sockaddr* sockets::sockaddr_cast(struct sockaddr_in6* addr)
{
    return static_cast<struct sockaddr*>(implicit_cast<void*>(addr));
}

const struct sockaddr_in* sockets::sockaddr_in_cast(const struct sockaddr* addr)
{
    return static_cast<const struct sockaddr_in*>(implicit_cast<const void*>(addr));
}

const struct sockaddr_in6* sockets::sockaddr_in6_cast(const struct sockaddr* addr)
{
    return static_cast<const struct sockaddr_in6*>(implicit_cast<const void*>(addr));
}


struct sockaddr_in6 sockets::getLocalAddr(int sockfd)
{
    struct sockaddr_in6 localaddr;
    bzero(&localaddr,sizeof(sockaddr_in6));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(localaddr));
    if(::getsockname(sockfd,sockaddr_cast(&localaddr),&addrlen) < 0)
    {
        fprintf(stderr,"sockets::getLocalAddr");
    }
    return localaddr;
}

struct sockaddr_in6 sockets::getPeerAddr(int sockfd)
{
    struct sockaddr_in6 peeraddr;
    bzero(&peeraddr,sizeof(sockaddr_in6));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(peeraddr));
    if(::getpeername(sockfd,sockaddr_cast(&peeraddr),&addrlen) < 0)
    {
        fprintf(stderr,"sockets::getPeerAddr");
    }
    return peeraddr;
}

#if !(__GNUC_PREREQ(4,6))
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
bool sockets::isSelfConnect(int sockfd)
{
    struct sockaddr_in6 localaddr = getLocalAddr(sockfd);
    struct sockaddr_in6 peeraddr = getPeerAddr(sockfd);
    if(localaddr.sin6_family == AF_INET)
    {
        const struct sockaddr_in* laddr4 = reinterpret_cast<struct sockaddr_in*>(&localaddr);
        const struct sockaddr_in* paddr4 = reinterpret_cast<struct sockaddr_in*>(&peeraddr);
        return laddr4->sin_port == paddr4->sin_port 
                && laddr4->sin_addr.s_addr == paddr4->sin_addr.s_addr;
    }
    else if(localaddr.sin6_family == AF_INET6)
    {
        return localaddr.sin6_port == peeraddr.sin6_port
            && ::memcmp(&localaddr.sin6_addr,&peeraddr.sin6_addr,sizeof(localaddr.sin6_addr)) == 0;
    }
}
