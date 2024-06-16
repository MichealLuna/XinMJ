
#include <xin/base/CurrentThread.h>
#include <xin/base/Exception.h>
#include <xin/base/Thread.h>

#include <type_traits>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

namespace xin
{
namespace CurrentThread
{
    __thread int _cacheTid;
    __thread char _tidString[48];
    __thread int _tidStringLength;
    __thread const char *_threadName;
}

namespace detail
{

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

void afterFork()
{
    xin::CurrentThread::_cacheTid = 0;
    xin::CurrentThread::_threadName = "main";
    CurrentThread::tid();
}

class ThreadNameInitializer
{
    public:
        ThreadNameInitializer()
        {
            xin::CurrentThread::_cacheTid = 0;
            CurrentThread::tid();
            pthread_atfork(NULL,NULL,&afterFork);
        }
};

ThreadNameInitializer init;

struct ThreadData
{
    typedef xin::Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    string name_;
    std::weak_ptr<pid_t> wkTid_;

    ThreadData(const ThreadFunc& func,
               const string& name,
               const std::shared_ptr<pid_t>& tid)
     : func_(func),
       name_(name),
       wkTid_(tid)
    { }

    void runInThread()
    {
        pid_t tid = xin::CurrentThread::tid();
        
        std::shared_ptr<pid_t> ptid = wkTid_.lock();
        if(ptid)
        {
            *ptid = tid;
            ptid.reset();
        }

        xin::CurrentThread::_threadName = name_.empty()? "xinThread" : name_.c_str();
        ::prctl(PR_SET_NAME,xin::CurrentThread::_threadName);

        try
        {
            func_();
            xin::CurrentThread::_threadName = "finished";
        }
        catch(const Exception& e)
        {
            xin::CurrentThread::_threadName = "crashed";
            fprintf(stderr,"exception caught in Thread %s\n",name_.c_str());
            fprintf(stderr,"reason: %s\n",e.what());
            fprintf(stderr,"stack trace : %s\n",e.stackTrace());
            abort();
        }
        catch(const std::exception& e)
        {
            xin::CurrentThread::_threadName = "crashed";
            fprintf(stderr,"exception caught in Thread %s\n",name_.c_str());
            fprintf(stderr,"reason: %s\n",e.what());
            abort();
        }
        catch(...)
        {
            xin::CurrentThread::_threadName = "crashed";
            fprintf(stderr,"unknown exception caught in Thread %s\n",name_.c_str());
            throw;
        }
    }
};

void* startThread(void* obj)
{
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}

} // namespace detail
} // namespace xin

using namespace xin;

void CurrentThread::cacheTid()
{
    if(_cacheTid == 0)
    {
        _cacheTid = detail::gettid();
        _tidStringLength = snprintf(_tidString,sizeof _tidString,"%05d",_cacheTid);
    }
}

bool CurrentThread::isMainThread()
{
    return tid() == ::gettid();
}

void CurrentThread::sleepUsec(int64_t usec)
{
    struct timespec ts = {0,0};
    ts.tv_sec = static_cast<time_t>(usec / 1000 * 1000);
    ts.tv_nsec = static_cast<long>(usec % 1000 * 1000 * 1000);
    ::nanosleep(&ts,NULL);
}

std::atomic<int32_t> Thread::numCreated_;

Thread::Thread(const ThreadFunc &func, const string &name)
  : started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(new pid_t(0)),
    func_(func),
    name_(name)
{
    setDefaultName();
}

Thread::Thread(ThreadFunc &&func, const string &name)
  : started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(new pid_t(0)),
    func_(std::move(func)),
    name_(name)
{
    setDefaultName();
}

Thread::~Thread()
{
    if(started_ && !joined_)
    {
        pthread_detach(pthreadId_);
    }
}

void Thread::setDefaultName()
{
    int num = ++numCreated_;
    if(name_.empty())
    {
        char buf[32];
        snprintf(buf,sizeof buf,"Thread%d",num);
        name_ = buf;
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    detail::ThreadData* data = new detail::ThreadData(func_,name_,tid_);
    if(pthread_create(&pthreadId_,NULL,&detail::startThread,data))
    {
        started_ = false;
        delete data;
    }
}

int Thread::join()
{
    assert(!joined_);
    assert(started_);
    joined_ = true;
    return pthread_join(pthreadId_,NULL);
}


