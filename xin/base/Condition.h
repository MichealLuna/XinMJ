#ifndef XINMJ_BASE_CONDITION_H
#define XINMJ_BASE_CONDITION_H

#include <xin/base/noncopyable.h>
#include <xin/base/Mutex.h>

namespace xin
{

class Condition : public noncopyable
{
 public:
  explicit Condition(Mutex& mtx)
   : mtx_(mtx)
  {
    XCHECK(pthread_cond_init(&cond_,NULL));
  }

  ~Condition()
  {
    XCHECK(pthread_cond_destroy(&cond_));
  }

  void wait()
  {
    Mutex::UnassignGuard ug(mtx_);
    XCHECK(pthread_cond_wait(&cond_,mtx_.getMutex()));
  }

  //return true if time out,otherwise false.
  bool waitForSeconds(double seconds);

  void notify()
  {
    XCHECK(pthread_cond_signal(&cond_));
  }

  void notifyAll()
  {
    XCHECK(pthread_cond_broadcast(&cond_));
  }

 private:
  Mutex& mtx_;
  pthread_cond_t cond_;
};

}//namespace xin

#endif//!XINMJ_BASE_CONDITION_H