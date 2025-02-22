#pragma once 

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <string>
#include "common.h"

const int kMicrosecond2Second = 1000 * 1000;


class TimeStamp {

    public:
        TimeStamp() : _micro_seconds(0) {}
        explicit TimeStamp(int64_t micro_seconds) : _micro_seconds(micro_seconds) {}

        bool operator<(const TimeStamp &rhs) const{
            return _micro_seconds < rhs.microseconds();
        }
        bool operator==(const TimeStamp &rhs) const{
            return _micro_seconds == rhs.microseconds();
        }

        std::string ToFormattedString() const {
            char buf[64] = {0};
            time_t seconds = static_cast<time_t>(_micro_seconds / kMicrosecond2Second);
            struct tm tm_time;
            localtime_r(&seconds, &tm_time);
            int microseconds = static_cast<int>(_micro_seconds % kMicrosecond2Second);
            snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d.%06d",
                    tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                    tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, microseconds);
            return buf;
        }
        int64_t microseconds() const { return _micro_seconds; };

        static TimeStamp Now();
        static TimeStamp AddTime(TimeStamp timestamp, double add_seconds);
    private:
        int64_t _micro_seconds;
};


inline TimeStamp TimeStamp::Now(){
    struct timeval time;
    gettimeofday(&time, NULL);
    return TimeStamp(time.tv_sec * kMicrosecond2Second + time.tv_usec);
};

inline TimeStamp TimeStamp::AddTime(TimeStamp timestamp, double add_seconds){
    int64_t add_microseconds = static_cast<int64_t>(add_seconds) * kMicrosecond2Second;   
    return TimeStamp(timestamp.microseconds() + add_microseconds);
};