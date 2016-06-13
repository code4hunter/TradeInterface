#ifndef _TIME_UTLITY_H_
#define _TIME_UTLITY_H_

#include <string>

void get_current_dt(int &d,int &t);

std::string get_current_dt_s(void);

std::string get_current_date_s(void);

__int64 get_current_dt(int &d,int &t, int &ms);
__int64 get_milliseconds(int t,int ms);

__int64 get_tick_count(void);

long second_between_time(long t1, long t2);

long second_between_trading_time(long old_tm, long tm);

#endif