//
// Created by ruiy on 18-3-13.
//
#include <stdio.h>
#include "date_utils.h"

/**
 * 将年月日转化为秒数
 * @param year
 * @param month
 * @param day
 * @return
 */
time_t convert(int year,int month,int day, int hour, int minute, int second)
{
    tm info={0};
    info.tm_year=year-1900;
    info.tm_mon=month-1;
    info.tm_mday=day;
    info.tm_hour=hour;
    info.tm_min=minute;
    info.tm_sec = second;
    return mktime(&info);
}

/**
 * 用于得到两天之间的时间差, to - from
 * @param from
 * @param to
 * @return
 */
int get_days(const char *from, const char *to)
{
    int year,month,day;
    sscanf(from,"%d-%d-%d",&year,&month,&day);
    int fromSecond=(int)convert(year,month,day);
    sscanf(to,"%d-%d-%d",&year,&month,&day);
    int toSecond=(int)convert(year,month,day);
    return (toSecond-fromSecond)/24/3600;
}

/**
 * 用于得到两个时间点的时间差, to - from
 * @param from
 * @param to
 * @return
 */
int get_hours(const char *from, const char *to)
{
    int year,month,day,hour,minute,second;
    sscanf(from,"%d-%d-%d %d:%d:%d",&year,&month,&day,&hour,&minute,&second);
    int fromSecond=(int)convert(year,month,day,hour,minute,second);

    sscanf(to,"%d-%d-%d %d:%d:%d",&year,&month,&day,&hour,&minute,&second);
    int toSecond=(int)convert(year,month,day,hour,minute,second);

    return (toSecond-fromSecond)/3600;
}

char* add_days(const char *from, int add_day)
{
    int year, month, day, hour, minute, second;
    sscanf(from,"%d-%d-%d %d:%d:%d",&year,&month,&day,&hour,&minute,&second);
    time_t fromSecond = convert(year,month,day, hour, minute, second);
    time_t toSecond = fromSecond + add_day * 24 * 3600;
    tm *tm = localtime(&toSecond);
    char *s = new char[20];
    sprintf(s, "%d-%.2d-%.2d %.2d:%.2d:%.2d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    return s;
}