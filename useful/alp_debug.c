/*** DEBUG BEGIN ***/
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#ifndef alp_debugLen
#define alp_debugLen 256
#endif
#ifndef alp_debugPrePath
#define alp_debugPrePath "/tmp/alp_debug_"
#endif
#ifndef alp_debug
#define alp_debug(a...) {	\
	alp_debug_func(__FILE__, __FUNCTION__, __LINE__, a); \
}
#endif

/* TODO: use portable one instead of __progname */
extern char *__progname;

static int
alp_debug_get_clock (void)
{
	int tsms;
	struct timespec timespec;
	clock_gettime(CLOCK_MONOTONIC, &timespec);
	tsms = (timespec.tv_sec * 1000) + (timespec.tv_nsec / 1000000);
	return tsms;
}

static void
alp_debug_func(const char *file, const char *func, int line, const char *format, ...)
{
	static char alp_debugPath[128] = {0};

	snprintf(alp_debugPath, 128, alp_debugPrePath"%s.txt", __progname);
	//if (access(alp_debugPath, F_OK | W_OK) == 0)
	{
		va_list args;
		char alp_debugBuf[alp_debugLen] = { '\0' };
		int n = 0;
		int alp_fdesc = 0;

		va_start(args, format);
		n += snprintf(alp_debugBuf,alp_debugLen-1,"%d\t%s\t%s\t%d\t| ", alp_debug_get_clock(), file, func, line);
		n += vsnprintf(alp_debugBuf+n,alp_debugLen-1, format, args)	;
		n += snprintf(alp_debugBuf+n,alp_debugLen-1,"\n");
		if ((alp_fdesc = open(alp_debugPath, O_WRONLY | O_APPEND | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) >= 0 ) {
			write(alp_fdesc, alp_debugBuf, n);
			close(alp_fdesc);	
		} else {
			fprintf(stderr, "OPEN FAIL: %s \n" , strerror(errno));
		}
		va_end (args);	
	}
}
/*** DEBUG END ***/
