#ifndef XINMJ_BASE_COUNTDOWNLATCH_H
#define XINMJ_BASE_COUNTDOWNLATCH_H

#include <xin/base/Condition.h>
#include <xin/base/Mutex.h>
#include <noncopyable.h>

using namespace xin;

namespace xin
{

class CountDownLatch : noncopyable
{
 public:
  explicit CountDownLatch(int count);

  void wait();

  void countDown();

  int getCount() const { return count_; }
  
 private:
  Mutex mtx_;
  Condition cond_;
  int count_;
};

}// namespace xin

#endif//!XINMJ_BASE_COUNTDOWNLATCH_H