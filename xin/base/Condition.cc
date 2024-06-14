#include <xin/base/Condition.h>
#include <errno.h>

using namespace xin;

bool Condition::waitForSeconds(double seconds)
{
    struct timespec tspec;
    clock_gettime(CLOCK_REALTIME,&tspec);
    const int64_t kNanoSecondsPerSeconds = 1e9;
    int64_t nanoseconds = static_cast<int64_t>(seconds * kNanoSecondsPerSeconds);

    tspec.tv_sec += static_cast<time_t>((tspec.tv_nsec + nanoseconds) / kNanoSecondsPerSeconds);
    tspec.tv_nsec = static_cast<int64_t>((tspec.tv_nsec + nanoseconds) % kNanoSecondsPerSeconds);
    Mutex::UnassignGuard ug(mtx_);
    return ETIMEDOUT == pthread_cond_timedwait(&cond_,mtx_.getMutex(),&tspec);
}