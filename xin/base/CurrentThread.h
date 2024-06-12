#ifndef XINMJ_BASE_CURRENTTHREAD_H
#define XINMJ_BASE_CURRENTTHREAD_H

#include <stdint.h>

namespace xin
{
    namespace CurrentThread
    {
        extern __thread int _cacheTid;
        extern __thread char _tidString[48];
        extern __thread int _tidStringLength;
        extern __thread const char* _threadName;
        void cacheTid();

        inline int tid()
        {
            if(__builtin_expect(_cacheTid == 0,0))
            {
                cacheTid();
            }
            return _cacheTid;
        }

        inline const char* tidString()
        {
            return _tidString;
        }

        inline int tidStringLength()
        {
            return _tidStringLength;
        }

        inline const char* name()
        {
            return _threadName;
        }

        //define in Thread.h
        bool isMainThread();

        void sleepUsec(int64_t usec);
    } // namespace CurrentThread
}

#endif //!XINMJ_BASE_CURRENTTHREAD_H