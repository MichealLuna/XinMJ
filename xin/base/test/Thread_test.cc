#include <xin/base/CurrentThread.h>
#include <xin/base/Thread.h>

#include <functional>
#include <stdio.h>
#include <string>
#include <unistd.h>

using namespace xin;
using namespace CurrentThread;

void selfSleep(int seconds)
{
    timespec t = {seconds,0};
    nanosleep(&t,NULL);
}

void threadFunc1()
{
    printf("tid = %d\n",xin::CurrentThread::tid());
}

void threadFunc2(int param)
{
    printf("tid = %d , param = %d\n",xin::CurrentThread::tid(),param);
}

void threadFunc3()
{
    printf("tid = %d\n",xin::CurrentThread::tid());
    selfSleep(1);
}

class Bar
{
    public:
     explicit Bar(int val)
      : val_(val)
    {
    }

    void memberFunc1()
    {
        printf("tid = %d, Bar::val_ = %d\n",CurrentThread::tid(),val_);
    }

    void memberFunc2(const std::string& text)
    {
        printf("tid = %d, Bar::val_ = %d,text = %s\n",CurrentThread::tid(),val_,text.c_str());
    }

    private:
        int val_;
};

int main()
{
    printf("pid = %d ,tid = %d\n",::getpid(),xin::CurrentThread::tid());

    Thread t1(threadFunc1);
    t1.start();
    t1.join();

    Thread t2(std::bind(threadFunc2,123),"thread for free func with parameter");
    t2.start();
    t2.join();

    Bar bar(32);
    Thread t3(std::bind(&Bar::memberFunc1,&bar),"thread for member func without parameter");
    t3.start();
    t3.join();

    Thread t4(std::bind(&Bar::memberFunc2,std::ref(bar),std::string("thread for member func with paramter")));
    t4.start();
    t4.join();

    {
        //t5 may destruct eariler than thread creation.
        Thread t5(threadFunc3);
        t5.start();
    }

    {
        //t6 destruct later than thread creation.
        Thread t6(threadFunc3);
        t6.start();
        selfSleep(3);
    }
    sleep(2);
    printf("number of created threads : %d\n",xin::Thread::numCreated());
}