#ifndef XINMJ_NET_CALLBACKS_H
#define XINMJ_NET_CALLBACKS_H

#include <xin/base/Timestamp.h>

#include <functional>
#include <memory>

namespace xin
{

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

template<typename T>
inline T* get_pointer(const std::shared_ptr<T>& ptr)
{
    return ptr.get();
}

template<typename T>
inline T* get_pointer(const std::unique_ptr<T>& ptr)
{
    return ptr.get();
}

template<typename To, typename From>
inline ::std::shared_ptr<To> down_pointer_cast(const ::std::shared_ptr<From>& f)
{
    if(false)
    {   //checking whether 'To''s base class is 'From'.
        implicit_cast<From*,To*>(0);
    }

#ifndef NDEBUG
    assert(f == NULL || dynamic_cast<To*>(get_pointer(f)) != NULL);
#endif
    return std::static_pointer_cast<To>(f);
}

namespace net
{

class Buffer;
class TcpConnection;
typedef std::shared_ptr<TcpConnection>  TcpConnectionPtr;
typedef std::function<void()>   TimerCallback;
typedef std::function<void (const TcpConnectionPtr&)>   ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)>   CloseCallback;
typedef std::function<void (const TcpConnectionPtr&)>   WriteCompleteCallback;
typedef std::function<void (const TcpConnectionPtr&)>   HighWaterMarkCallback;

typedef std::function<void (const TcpConnectionPtr&,
                                  Buffer*,
                                  Timestamp)>   MessageCallback;

void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn,
                            Buffer* buffer,
                            Timestamp receiveTime);

}//namespace net
}//namespace xin1

#endif//!XINMJ_NET_CALLBACKS_H