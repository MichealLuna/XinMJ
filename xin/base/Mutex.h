#ifndef XINMJ_BASE_MUTEXT_H
#define XINMJ_BASE_MUTEXT_H

#include <xin/base/CurrentThread.h>
#include <xin/base/noncopyable.h>
#include <assert.h>
#include <pthread.h>

#ifdef CHECK_PTHREAD_RETURN_VALUE
#ifdef NDEBUG
__BEGIN_DECLS
extern void __assert_perror_fail(int errnum,
                                 const char *file,
                                 unsigned int line,
                                 const char* function)
    __THROW __attribute__ ((__noreturn__));
__END_DECLS
#endif
#define XCHECK(ret) ({  __typeof__ (ret) errnum = (ret);    \
                        if(__builtin_expect(errnum != 0,0)) \
                            __assert_perror_fail(errnum,__FILE__,__LINE__,__func__);})
#else //!CHECK_PTHREAD_RETURN_VALUE

#define XCHECK(ret) ({ __typeof__(ret) errnum = (ret);      \
                        assert(errnum == 0); (void) errnum;})
#endif //!CHECK_PTHREAD_RETURN_VALUE

namespace xin
{

class Mutex : noncopyable
{
 public:
  Mutex()
   : holder_(0)
  {
    XCHECK(pthread_mutex_init(&mutex_,NULL));
  }
 
  ~Mutex()
  {
    assert(holder_ == 0);
    XCHECK(pthread_mutex_destroy(&mutex_));
  }
  
  bool isLockedByThisThread() const
  {
    return holder_ == CurrentThread::tid();
  }
 
  void assertLocked() const
  {
    assert(isLockedByThisThread());
  }

  //internel
  void lock()
  {
    XCHECK(pthread_mutex_lock(&mutex_));
    assignHolder();
  }

  void unlock()
  {
    unassignHolder();
    XCHECK(pthread_mutex_unlock(&mutex_));
  }

  pthread_mutex_t* getMutex()
  {
    return &mutex_;
  }

 private:
    pthread_mutex_t mutex_;
    pid_t holder_;

    friend class Condition;

private:
    void assignHolder()
    {
        holder_ = CurrentThread::tid();
    }

    void unassignHolder()
    {
        holder_ = 0;
    }
    class UnassignGuard : noncopyable
    {
     public:
        UnassignGuard(Mutex& owner)
         : owner_(owner)
        {
            owner_.unassignHolder();
        }

        ~UnassignGuard()
        {
            owner_.assignHolder();
        }
     private:
        Mutex& owner_;
    };
};

//use as stack variable.
class MutexLockGuard : noncopyable
{
 public:
  explicit MutexLockGuard(Mutex& mtx)
   : mutex_(mtx)
  {
    mutex_.lock();
  }

  ~MutexLockGuard()
  {
    mutex_.unlock();
  }
 private:
    Mutex& mutex_;
};

}//namespace xin

#define MutexLockGuard(x) error "Missing guard object name"

#endif//!XINMJ_BASE_MUTEXT_H