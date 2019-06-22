#ifndef __STOPWATCH_HEADER_INCLUDED__
#define __STOPWATCH_HEADER_INCLUDED__
#include <time.h>

typedef struct {
  struct timespec start_ts;
  struct tm start_tm;
  struct timespec end_ts;
  struct tm end_tm;
} StopWatch;

void StopWatch_start(StopWatch *sw);
void StopWatch_stop(StopWatch *sw);

typedef struct {
  int hour;
  int min;
  int sec;
  long nsec;
} StopWatchDiff;

StopWatchDiff StopWatch_get_diff(StopWatch *sw);
double swd_to_double(StopWatchDiff *swd);
#endif