#include <xin/base/Timestamp.h>

#include <sys/time.h>
#include <stdio.h>

#include <inttypes.h>

using namespace xin;

static_assert(sizeof(Timestamp) == sizeof(int64_t),
                "Timestamp is same size as int64_t");

string Timestamp::toString() const
{
    char buf[64] = {0};
    int64_t seconds = microSecondsSinceEpoch_ / cMicroSecondsPerSecond;
    int64_t microseconds = microSecondsSinceEpoch_ % cMicroSecondsPerSecond;
    snprintf(buf,sizeof(buf) - 1,"%" PRId64 ".%06" PRId64 "",seconds,microseconds);
    return buf;
}

string Timestamp::toFormattedString(bool showMicroSeconds) const
{
    char buf[64] = {0};
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / cMicroSecondsPerSecond);
    struct tm tim;
    gmtime_r(&seconds,&tim);

    if(showMicroSeconds)
    {
        int microseconds = static_cast<int>(microSecondsSinceEpoch_ % cMicroSecondsPerSecond);
        snprintf(buf,sizeof(buf),"%4d%02d%02d %02d:%02d:%02d.%06d",
                    1900 + tim.tm_year,tim.tm_mon + 1,tim.tm_mday,
                    tim.tm_hour,tim.tm_min,tim.tm_sec,microseconds);
    }
    else
    {
        snprintf(buf,sizeof(buf),"%4d%02d%02d %02d:%02d:%02d",
            1900 + tim.tm_year,tim.tm_mon + 1,tim.tm_mday,
            tim.tm_hour,tim.tm_min,tim.tm_sec);
    }
    return buf;
}

Timestamp Timestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds * Timestamp::cMicroSecondsPerSecond + tv.tv_usec);
}