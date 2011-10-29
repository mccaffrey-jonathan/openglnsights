#include "platform/time.h"

#include <stdlib.h>
#include <sys/time.h>

int64_t TimeMillis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t sec = tv.tv_sec;
    int64_t usec = tv.tv_usec;
    return sec*1000 + usec/1000;
}
