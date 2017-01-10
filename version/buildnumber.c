/*******************************************************************
* simple tool for generating build number from current date and time
* Copyright (C) 2010-2017 Michael M. Builov
*******************************************************************/

/* buildnumber.c */

#include <time.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	time_t current = time(NULL);
	const struct tm *t = localtime(&current);
	unsigned buildnum = (unsigned)((t->tm_year % 100)*1000 + (t->tm_mon + 1)*64 + t->tm_mday*2 + t->tm_hour/12) & 65535;
	printf("%u %u/%02u/%02u %02u:%02u:%02u", buildnum, (unsigned)t->tm_year + 1900, (unsigned)t->tm_mon + 1,
		(unsigned)t->tm_mday, (unsigned)t->tm_hour, (unsigned)t->tm_min, (unsigned)t->tm_sec);
	(void)argc, (void)argv;
	return 0;
}
