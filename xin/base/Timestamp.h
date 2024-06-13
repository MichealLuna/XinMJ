#ifndef XINMJ_BASE_TIMESTAMP_H
#define XINMJ_BASE_TIMESTAMP_H

#include <xin/base/copyable.h>
#include <xin/base/Types.h>

namespace xin
{

class Timestamp : public xin::copyable
{
public:
    Timestamp()
      : microSecondsSinceEpoch_(0)
    {
    }
    
    explicit Timestamp(int64_t microSecondsSinceEpoch)
     : microSecondsSinceEpoch_(microSecondsSinceEpoch)
    {
    }

    void swap(Timestamp& other)
    {
        std::swap(microSecondsSinceEpoch_,other.microSecondsSinceEpoch_);
    }

    string toString() const;
    string toFormattedString(bool showMicroSeconds = true) const;

    bool valid() const { return microSecondsSinceEpoch_ > 0; }

    //internel usage
    static const int cMicroSecondsPerSecond = 1000000;
    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
    time_t secondsSinceEpoch() const 
    { return static_cast<time_t>(microSecondsSinceEpoch_ / cMicroSecondsPerSecond); }

    static Timestamp now();
    static Timestamp invalid()
    {
        return Timestamp();
    }

    //unix time is in second.
    static Timestamp fromUnixTime(time_t t)
    {
        return fromUnixTime(t,0);
    }

    static Timestamp fromUnixTime(time_t t,int microseconds)
    {
        return Timestamp(static_cast<int64_t>(t) * cMicroSecondsPerSecond + microseconds);
    }

    friend bool operator==(Timestamp lhs,Timestamp rhs);
    friend bool operator<(Timestamp lhs,Timestamp rhs);
private:
    int64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp lhs,Timestamp rhs)
{
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs,Timestamp rhs)
{
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

//return difference of two timestamps in seconds.
inline double timeDifference(Timestamp high,Timestamp low)
{
    int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff / Timestamp::cMicroSecondsPerSecond);
}

inline Timestamp addTime(Timestamp timestamp,double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::cMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

}// namespace xin

#endif//!XINMJ_BASE_TIMESTAMP_H