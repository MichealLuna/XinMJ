#include <xin/base/Timestamp.h>
#include <vector>

using namespace xin;

void showTimestampByRef(const Timestamp& stamp)
{
    printf("stamp : %s\n",stamp.toString().c_str());
}

void showTimestampByValue(Timestamp stamp)
{
    printf("stamp : %s\n",stamp.toString().c_str());
}

void benchmark()
{
    const int cNumber = 1000000;

    std::vector<Timestamp> stamps;
    stamps.reserve(cNumber);
    for(int i = 0; i < cNumber; ++i)
    {
        stamps.push_back(Timestamp::now());
    }

    printf("stamps front : %s\n",stamps.front().toString().c_str());
    printf("stamps back  : %s\n",stamps.back().toString().c_str());
    printf("back - front : %fs\n",timeDifference(stamps.front(),stamps.back()));

    int incrememts[100] = { 0 };
    int64_t start = stamps.front().microSecondsSinceEpoch();
    for(int i = 1; i < cNumber; ++i)
    {
        int64_t next = stamps[i].microSecondsSinceEpoch();
        int64_t inc  = next - start;
        start = next;
        if(inc < 0)
        {
            printf("reverse!\n");
        }
        else if(inc < 100)
        {
            ++incrememts[inc];
        }
        else
        {
            printf("big gap %d\n",static_cast<int>(inc));
        }
    }

    for(int i = 0; i < 100; ++i)
    {
        printf("%2d : %d\n",i,incrememts[i]);
    }
}

int main()
{
    Timestamp now(Timestamp::now());
    printf("%s\n",now.toFormattedString().c_str());
    showTimestampByRef(now);
    showTimestampByValue(now);
    benchmark();
}