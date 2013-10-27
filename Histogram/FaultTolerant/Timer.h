
#pragma once

#include <sys/time.h>


class Timer
{

    struct timeval beginTime, endTime, diffTime;

    int GetDiff()
    {
        long int diff = (endTime.tv_usec + 1000000 * endTime.tv_sec) - (beginTime.tv_usec + 1000000 * beginTime.tv_sec);
        diffTime.tv_sec = diff / 1000000;
        diffTime.tv_usec = diff % 1000000;

        return (diff < 0);
    }

public:

    void start();
    void end();
    void print();

};

