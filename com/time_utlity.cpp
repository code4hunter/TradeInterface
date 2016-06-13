#include "time_utlity.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <time.h>

void get_current_dt(int &d,int &t)
{
	time_t timer;
	struct tm * timeinfo;
	
	time(&timer); 
	timeinfo = localtime ( &timer );
	d = (1900+timeinfo->tm_year)*10000+(timeinfo->tm_mon+1)*100 + timeinfo->tm_mday;
	t = timeinfo->tm_hour*10000+timeinfo->tm_min*100+timeinfo->tm_sec;
}

std::string get_current_dt_s(void)
{
	time_t timer;
	struct tm * timeinfo;

	time(&timer);
	timeinfo = localtime(&timer);
	char buf[100];
	int i = _snprintf(buf,100, "%d-%02d-%02d %02d:%02d:%02d", (1900 + timeinfo->tm_year), timeinfo->tm_mon + 1, timeinfo->tm_mday,
		timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	if (i > 0)
	{
		buf[i] = 0;
		return std::string(buf);
	}
	else return "";
}

std::string get_current_date_s(void)
{
	time_t timer;
	struct tm * timeinfo;

	time(&timer);
	timeinfo = localtime(&timer);
	char buf[100];
	int i = _snprintf(buf, 100, "%d-%02d-%02d", (1900 + timeinfo->tm_year), timeinfo->tm_mon + 1, timeinfo->tm_mday);
	if (i > 0)
	{
		buf[i] = 0;
		return std::string(buf);
	}
	else return "";
}

__int64 get_current_dt(int &d,int &t, int &ms)
{
	boost::posix_time::ptime pt = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration td= pt.time_of_day();
	boost::gregorian::date dt = pt.date();

	d = dt.year() * 10000 + dt.month() * 100 + dt.day();
	t = td.hours()* 10000 + td.minutes() * 100 + td.seconds();
	ms = td.total_milliseconds() % 1000;
	return td.total_milliseconds();
}

__int64 get_milliseconds(int t,int ms)
{
	int h = t/10000;
	int m = t%10000/100;
	int s = t%100;
	return (h*60*60+m*60+s)*1000 + ms;
}

__int64 get_tick_count(void)
{
	boost::posix_time::ptime pt = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration td= pt.time_of_day();
	return td.total_milliseconds();
}

long second_between_time(long t1, long t2)
{
	long s1 = t1 / 10000 * 60 * 60 + t1 % 10000 / 100 * 60 + t1 % 100;
	long s2 = t2 / 10000 * 60 * 60 + t2 % 10000 / 100 * 60 + t2 % 100;
	return s2 - s1;
}

long second_between_trading_time(long old_tm, long tm)
{
	int nsecond = 0;
	for (int i = old_tm; i<tm; i++)
	{
		if ((i >= 93000 && i < 113000) || (i >= 130000 && i < 150000)) {
			int m = i % 10000 / 100;
			int s = i % 100;
			if (m < 60 && s < 60) {
				nsecond++;
			}
		}
	}
	return nsecond;
}