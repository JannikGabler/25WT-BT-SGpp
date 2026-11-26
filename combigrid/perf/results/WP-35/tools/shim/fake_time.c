// LD_PRELOAD shim: time() returns $FAKE_TIME (seconds) if set, else the real time.
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>
#include <time.h>
time_t time(time_t* t) {
  const char* s = getenv("FAKE_TIME");
  time_t v;
  if (s) {
    v = (time_t)strtoll(s, NULL, 10);
  } else {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    v = ts.tv_sec;
  }
  if (t) *t = v;
  return v;
}
