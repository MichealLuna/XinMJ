#include <xin/base/Mutex.h>
#include <xin/base/Thread.h>
#include <xin/base/Timestamp.h>

#include <functional>
#include <vector>
#include <memory>
#include <stdio.h>

using namespace std;
using namespace xin;

int g_val = 0;
Mutex mtx_val;
vector<int> g_vec;

int cNumber = 10 * 1000 * 1000;


void threadFunc()
{
    for(int i = 0; i < cNumber; ++i)
    {
        MutexLockGuard mtx(mtx_val);
        g_vec.push_back(i);
    }
}

int foo() __attribute__ ((noinline));

int foo()
{
    MutexLockGuard lock(mtx_val);
    if(!mtx_val.isLockedByThisThread())
    {
        printf("Fail");
        return -1;
    }
    ++g_val;
    return 0;
}

int main()
{
    XCHECK(foo());
    if(g_val != 1)
    {
        printf("XCHECK calls twice.\n");
        abort();
    }

    const int kMaxThreads = 8;
    g_vec.reserve(kMaxThreads * cNumber);

    Timestamp start(Timestamp::now());
    for(int i = 0; i < cNumber; ++i)
    {
        g_vec.push_back(i);
    }
    printf("single thread without lock %.3fs\n",timeDifference(Timestamp::now(),start));
    start = Timestamp::now();
    threadFunc();
    printf("single thread with lock %.3fs\n",timeDifference(Timestamp::now(),start));

    for(int nthreads = 1; nthreads < kMaxThreads; ++nthreads)
    {
        vector<std::unique_ptr<Thread>> threads;
        g_vec.clear();
        start = Timestamp::now();
        for(int i = 0; i < nthreads; ++i)
        {
            threads.push_back(unique_ptr<Thread>(new Thread(&threadFunc)));
            threads.back()->start();
        }

        for(int i = 0; i < nthreads; ++i)
        {
            threads[i]->join();
        }
        printf("%d threads with lock %.3fs\n",nthreads,timeDifference(Timestamp::now(),start));
    }
    return 0;
}