#pragma once
#include "time.h"
#include <string>
#include <iostream>

using namespace std;
typedef unsigned int            uint;
typedef unsigned long long      uint64;
typedef long long int           int64;

enum {
    FIRST_YEAR = -4713,
    FIRST_MONTH = 1,
    FIRST_DAY = 2,  // ### Qt 5: make FIRST_DAY = 1, by support jd == 0 as valid
    SECS_PER_DAY = 86400,
    MSECS_PER_DAY = 86400000,
    SECS_PER_HOUR = 3600,
    MSECS_PER_HOUR = 3600000,
    SECS_PER_MIN = 60,
    MSECS_PER_MIN = 60000,
    JULIAN_DAY_FOR_EPOCH = 2440588 // result of julianDayFromGregorianDate(1970, 1, 1)
};
class PTime;
class PDateTime;

class PDate
{
    friend class PDateTime;
public:
    PDate() : jd(0) {}
    PDate(int y, int m, int d);
    bool isNull() const {
        return jd == 0;
    }
    bool isValid() const {
        return !isNull();
    }

    int year() const;
    int month() const;
    int day() const;
    int dayOfWeek() const;
    int dayOfYear() const;
    int daysInMonth() const;
    int daysInYear() const;
    int weekNumber(int *yearNum = 0) const;

    bool setDate(int year, int month, int day);
    void getDate(int *year, int *month, int *day);

    PDate addDays(int days) const;
    PDate addMonths(int months) const;
    PDate addYears(int years) const;
    int daysTo(const PDate &) const;

    bool operator==(const PDate &other) const {
        return jd == other.jd;
    }
    bool operator!=(const PDate &other) const {
        return jd != other.jd;
    }
    bool operator<(const PDate &other) const {
        return jd < other.jd;
    }
    bool operator<=(const PDate &other) const {
        return jd <= other.jd;
    }
    bool operator>(const PDate &other) const {
        return jd > other.jd;
    }
    bool operator>=(const PDate &other) const {
        return jd >= other.jd;
    }
    PDate& operator=(const PDate& other) {
        this->jd = other.jd; return *this;
    }
    static PDate currentDate();

    // yyyy-mm-dd
    static PDate fromString(const string &s);
    string toString();
    static bool isLeapYear(int year);
    static bool isValid(int y, int m, int d);

private:
    // ### Qt 5: remove these two functions
    static uint gregorianToJulian(int y, int m, int d);
    static void julianToGregorian(uint jd, int &y, int &m, int &d);

    static inline PDate fromJulianDay(int jd) {
        PDate d; d.jd = jd; return d;
    }
    inline int toJulianDay() const {
        return jd;
    }

private:
    uint jd;
};


class PTime
{
    friend class PDateTime;
public:
    PTime() : mds(NullTime) {}
    PTime(int h, int m, int s = 0, int ms = 0);

    bool isNull() const {
        return mds == NullTime;
    }
    bool isValid() const;

    bool operator==(const PTime &other) const {
        return mds == other.mds;
    }
    bool operator!=(const PTime &other) const {
        return mds != other.mds;
    }
    bool operator<(const PTime &other) const {
        return mds < other.mds;
    }
    bool operator<=(const PTime &other) const {
        return mds <= other.mds;
    }
    bool operator>(const PTime &other) const {
        return mds > other.mds;
    }
    bool operator>=(const PTime &other) const {
        return mds >= other.mds;
    }

    bool setHMS(int h, int m, int s, int ms = 0);
    int hour() const;
    int minute() const;
    int second() const;
    int msec() const;

    PTime addSecs(int secs) const;
    int secsTo(const PTime &) const;

    PTime addMSecs(int ms) const;
    int msecsTo(const PTime &) const;

    static bool isValid(int h, int m, int s, int ms = 0);
    static PTime currentTime();
    // hh:mm:ss
    static PTime fromString(const string &s);
    string toString();

private:
    void start();
    int restart();
    int elapsed() const;

private:
    enum TimeFlag {
        NullTime = -1
    };
    inline int ds() const {
        return mds == -1 ? 0 : mds;
    }
    int mds;
};

class PDateTime
{
public:
    PDateTime();
    explicit PDateTime(const PDate &);
    PDateTime(const PDate &, const PTime &);
    PDateTime(const PDateTime &other);
    ~PDateTime();

    PDateTime &operator=(const PDateTime &other);
    bool operator==(const PDateTime &other) const;
    bool operator!=(const PDateTime &other) const;
    bool operator<(const PDateTime &other) const;
    bool operator<=(const PDateTime &other) const;
    bool operator>(const PDateTime &other) const;
    bool operator>=(const PDateTime &other) const;

    bool isNull() const;
    bool isValid() const;

    PDate date() const;
    PTime time() const;

    void setDate(const PDate &date);
    void setTime(const PTime &time);

    void setTime_t(time_t t);
    int64 toTime_t() const;

    // yyyy-mm-dd hh:mm:ss
    static PDateTime fromString(const string &s);
    static PDateTime currentDateTime();
    static PDateTime fromTime_t(time_t t);
    string toString();

    PDateTime addYears(int years) const;
    PDateTime addMonths(int months) const;
    PDateTime addDays(int days) const;
    PDateTime addSecs(int secs) const;

private:
    int64 toMSecsSinceEpoch() const;

private:
    PDate _date;
    PTime _time;
    tm _dateTime;

    uint64	d;
    static time_t  deltaTime;   // diff between utc and local seconds
};



