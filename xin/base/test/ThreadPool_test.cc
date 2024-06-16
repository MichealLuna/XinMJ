#include <xin/base/CountDownLatch.h>
#include <xin/base/ThreadPool.h>

#include <math.h>

using namespace xin;

const size_t factorial[] = {
 1,2,6,24,120,720,5040
};

static const size_t N = sizeof(factorial)/sizeof(size_t);

double thePowerOfe(double x)
{
  double ret = 1.0 + x;
  double powerBuf[N] = {1.0};
  powerBuf[0] = x;
  for(size_t i = 1; i < N; ++i)
  {
    powerBuf[i] = powerBuf[i-1] * x;
    ret += (powerBuf[i] / (double)factorial[i]);
  }
  return ret;
}

void nothing(double x)
{
  printf("e^(%0.1f) ~= %0.5f\n",x,thePowerOfe(x)); 
}

void task()
{
    double xbegin = 0.0;
    double step = 0.1;
    double xend = 3.0;
    for(double i = xbegin; i < xend; i += step)
    {
        printf("e^(%0.1f) ~= %0.5f\n",i,thePowerOfe(i));
    }
}

void printTask(const string& str)
{
    printf("%s\n",str.c_str());
}

void test(size_t maxTaskQueue)
{
  printf("Max queue size : %ld\n",maxTaskQueue);
  ThreadPool pool("MainThreadpool");
  pool.setMaxQueueSize(maxTaskQueue);
  pool.start(3);

  printf("Adding...\n");
  //the std::bind can change other function to void() type,
  //so that I can match the argument list.
  pool.run(std::bind(task));
  pool.run(std::bind(nothing,2));

  for(int i = 0; i < 10; ++i)
  {
    char buf [16] = {0};
    snprintf(buf,sizeof buf,"Doing Task%d...",i + 1);
    pool.run(std::bind(printTask,buf));
  }

  CountDownLatch latch(1);
  pool.run(std::bind(&CountDownLatch::countDown,&latch));
  latch.wait();
  pool.stop();
}

int main()
{
    test(0);
    test(1);
    test(10);
    test(100);
    return 0;
}