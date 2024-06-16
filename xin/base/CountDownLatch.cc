#include <xin/base/CountDownLatch.h>

using namespace xin;

CountDownLatch::CountDownLatch(int count)
 : mtx_(),
 cond_(mtx_), 
 count_(count)
{
}

void CountDownLatch::wait()
{
    MutexLockGuard lock(mtx_);
    while(count_ > 0)
        cond_.wait();
}

void CountDownLatch::countDown()
{
    MutexLockGuard lock(mtx_);
    --count_;
    if(count_ == 0)
        cond_.notifyAll();
}