#include <xin/base/ThreadPool.h>

using namespace xin;

explicit ThreadPool::ThreadPool(const string& name)
 : name_(name),
 running_(false),
 nThreads(0),
 maxQueueSize_(0),
 mtx_(),
 notEmpty_(mtx_),
 notFull_(mtx_)
{
}

ThreadPool::~ThreadPool()
{
  if(running_)
    stop();
}


void ThreadPool::start(int nThreads)
{
  assert(threads_.empty());
  running_ = true;
  threads_.reserve(nThreads);
  for(int i = 0; i < nThreads; ++i)
  {
    char threadId[32];
    snprintf(threadId,sizeof threadId,"%d",i + 1);
    threads_.push_back(std::unique_ptr<xin::Thread>(new xin::Thread(
        std::bind(&ThreadPool::runInThread,this),name_ + threadId)));
    threads_[i]->start();
  }

  if(nThreads == 0 && threadInitCallback_)
    threadInitCallback_();
}

void ThreadPool::stop()
{
  {
    MutexLockGuard lock(mtx_);
    running_ = false;
    notEmpty_.notify();
  }

  for(auto it = threads_.begin(); it != threads_.end(); ++it)
    (*it)->join();
}


size_t ThreadPool::taskQueueSize() const
{
  MutexLockGuard lock(mtx_);
  return tasks_.size();
}

  
//it could block when taskQueue is full;
void ThreadPool::run(const Task& func)
{
  if(tasks_.empty())
  {
    func();
  }
  else
  {
    MutexLockGuard lock(mtx_);
    while(isFull())
    {
        notFull_.wait();
    }
    assert(!isFull());
    tasks_.push_back(func);
    notEmpty_.notify();
  }
}

void ThreadPool::run(Task&& func)
{
  if(tasks_.empty())
  {
    func();
  }
  else
  {
    MutexLockGuard lock(mtx_);
    while(isFull())
    {
        notFull_.wait();
    }
    assert(!isFull());
    tasks_.push_back(std::move(func));
    notEmpty_.notify();
  }
}

bool ThreadPool::isFull() const
{
  mtx_.assertLocked();
  return maxQueueSize_ > 0 && tasks_.size() >= maxQueueSize_;
}

void ThreadPool::runInThread()
{
  try
  {
    if(threadInitCallback_)
    {
        threadInitCallback_();
    }

    while(running_)
    {
        Task task(take());
        if(task)
        {
            task();
        }
    }
  }
  catch(const xin::Exception& e)
  {
    fprintf(stderr,"Exception caught in ThreadPool %s\n",name_.c_str());
    fprintf(stderr,"Reason: %s\n",e.what());
    fprintf(stderr,"Stack Trace: %s\n",e.stackTrace());
    abort();
  }
  catch(const std::exception& e)
  {
    fprintf(stderr,"Exception caught in ThreadPool %s\n",name_.c_str());
    fprintf(stderr,"Reason: %s\n",e.what());
    abort();
  }
  catch(...)
  {
    fprintf(stderr,"Unknown exception caught in ThreadPool %s\n",name_.c_str());
    throw;
  }
  
}

ThreadPool::Task ThreadPool::take()
{
  MutexLockGuard lock(mtx_);
  while(tasks_.empty() && running_)
  {
    notEmpty_.wait();
  }
  
  Task task;
  if(!tasks_.empty())
  {
    task = tasks_.front();
    tasks_.pop_front();
    if(maxQueueSize_ > 0)
    {
        notFull_.notify();
    }
  }
  return task;
}
