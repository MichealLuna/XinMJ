#ifndef XINMJ_NET_INETADDRESS_H
#define XINMJ_NET_INETADDRESS_H

#include <xin/base/copyable.h>
#include <xin/base/StringPiece.h>

#include <netinet/in.h>

namespace xin
{
namespace net
{

namespace sockets
{
const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);
}

class InetAddress : public xin::copyable
{
 public:
  /*Constructs with given port number.*/
  explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false,bool ipv6 = false);

  /*Constructs with ip that should be like "1.1.1.1" and a port number.*/
  InetAddress(StringArg ip, uint16_t port,bool ipv6 = false);

  explicit InetAddress(const struct sockaddr_in& addr)
   : addr_(addr){}

  explicit InetAddress(const struct sockaddr_in6& addr)
   : addr6_(addr){}

  sa_family_t family() const { return addr_.sin_family; }
  string toIp() const;
  string toIpPort() const;
  uint16_t toPort() const;

  const struct sockaddr* getSockAddr() const { return sockets::sockaddr_cast(&addr6_); }
  void setSockAddrInet6(const sockaddr_in6& addr6) { addr6_ = addr6; }

  uint32_t ipNetEndian() const;
  uint16_t portNetEndian() const { return addr_.sin_port; }

  /*return true if success.*/
  static bool resolve(StringArg hostname,InetAddress* addr);

 private:
  union 
  {
    struct sockaddr_in addr_;
    struct sockaddr_in6 addr6_;
  };
};

}//namespace net;
}//namespace xin;

#endif//!XINMJ_NET_INETADDRESS_H