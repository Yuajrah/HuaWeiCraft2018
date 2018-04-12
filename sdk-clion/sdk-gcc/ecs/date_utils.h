//
// Created by ruiy on 18-3-13.
//

#ifndef SDK_GCC_DATE_UTILS_H
#define SDK_GCC_DATE_UTILS_H
#include <ctime>
int  get_days(const char* from, const char* to);
int get_hours(const char *from, const char *to);

time_t convert(int year,int month,int day, int hour=0, int minute=0, int second=0);
char* add_days(const char* from, int add_day);
#endif //SDK_GCC_DATE_UTILS_H
