#include "stopwatch.h"
#include <time.h>

void StopWatch_start(StopWatch *sw) {
  clock_gettime(CLOCK_REALTIME, &sw->start_ts);
  localtime_r(&sw->start_ts.tv_sec, &sw->start_tm);
}

void StopWatch_stop(StopWatch *sw) {
  clock_gettime(CLOCK_REALTIME, &sw->end_ts);
  localtime_r(&sw->end_ts.tv_sec, &sw->end_tm);
}

StopWatchDiff StopWatch_get_diff(StopWatch *sw) {
  StopWatchDiff swd = {sw->end_tm.tm_hour - sw->start_tm.tm_hour,
                       sw->end_tm.tm_min - sw->start_tm.tm_min,
                       sw->end_tm.tm_sec - sw->start_tm.tm_sec,
                       sw->end_ts.tv_nsec - sw->start_ts.tv_nsec};
  return swd;
}

double swd_to_double(StopWatchDiff *swd) {
  double ret = 0;

  ret += swd->hour * 60 * 60;
  ret += swd->min * 60;
  ret += swd->sec;
  ret += (swd->nsec * 0.000000001);

  return ret;
}
