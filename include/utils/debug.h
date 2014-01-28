#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#define VERBOSE_DEBUG 	2
#define VERBOSE_INFO	1
#define VERBOSE_NONE	0

#include <sys/time.h>
void get_time_interval(struct timeval * tdata);

#ifndef DEBUG_DISABLED

extern int verbose;

#define VERBOSE_ISINFO() (verbose==VERBOSE_INFO)
#define VERBOSE_ISDEBUG() (verbose==VERBOSE_DEBUG)

#define PRINT_DEBUG verbose=VERBOSE_DEBUG
#define PRINT_INFO verbose=VERBOSE_INFO
#define PRINT_NONE verbose=VERBOSE_NONE

#define DEBUG(_fmt, ...) if (verbose >= VERBOSE_DEBUG) \
	fprintf(stdout, "[DEBUG]: " _fmt, __VA_ARGS__)

#define INFO(_fmt, ...) if (verbose >= VERBOSE_INFO) \
	fprintf(stdout, "[INFO]:  " _fmt, __VA_ARGS__)

#else

#define DEBUG
#define INFO

#endif

#endif
