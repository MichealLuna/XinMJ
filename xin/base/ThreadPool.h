#ifndef XINMJ_BASE_THREADPOOL_H
#define XINMJ_BASE_THREADPOOL_H

#include <xin/base/Condition.h>
#include <xin/base/Mutex.h>
#include <xin/base/noncopyable.h>
#include <xin/base/Exception.h>
#include <xin/base/Thread.h>
#include <xin/base/Types.h>

#include <deque>
#include <memory>
#include <functional>
#include <vector>

namespace xin
{

class ThreadPool : noncopyable
{
 public:
  typedef std::function<void()> Task;
  
  explicit ThreadPool(const string& name = "Threadpool");
  ~ThreadPool();
  
  //must be set before start().
  void setMaxQueueSize(int maxSz) { maxQueueSize_ = maxSz; }
  void setThreadInitCallback(const Task& func)
  { threadInitCallback_ = func; }

  void start(int nThreads);
  void stop();

  const string& name() const { return name_; }

  size_t taskQueueSize() const;
  
  //it could block when taskQueue is full;
  void run(const Task& func);
  void run(Task&& func);

 private:
  bool isFull() const;
  void runInThread();
  Task take();

 private:
  string name_;
  bool running_;
  int nThreads;
  size_t maxQueueSize_;
  Task threadInitCallback_;
  mutable Mutex mtx_;
  Condition notEmpty_;
  Condition notFull_;
  std::deque<Task> tasks_;
  std::vector<std::unique_ptr<Thread>> threads_;
};

}//namespace xin

#endif//!XINMJ_BASE_THREADPOOL_H