#include "windows_time.h"
#include "time.h"
int PX_TimeGetYear()
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);

	return 1900+p->tm_year;
}

int PX_TimeGetMouth()
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);

	return 1900+p->tm_year;
}

int PX_TimeGetDay()
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);

	return 1900+p->tm_mon;
}

int PX_TimeGetHour()
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);

	return 1900+p->tm_hour;
}

int PX_TimeGetMinute()
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);

	return 1900+p->tm_min;
}

int PX_TimeGetSecond()
{
	time_t timep;
	struct tm *p;
	time (&timep);
	p=gmtime(&timep);

	return 1900+p->tm_sec;
}

