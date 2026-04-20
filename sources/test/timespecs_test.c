#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>

int main(void) {
    struct timespec ts;

    // Get current time from CLOCK_REALTIME (wall clock)
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        fprintf(stderr, "clock_gettime failed: %s\n", strerror(errno));
        return 1;
    }

    // Convert seconds to local time
    struct tm local_time;
    if (localtime_r(&ts.tv_sec, &local_time) == NULL) {
        fprintf(stderr, "localtime_r failed\n");
        return 1;
    }

    // Format date/time string
    char time_str[64];
    if (strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &local_time) == 0) {
        fprintf(stderr, "strftime failed\n");
        return 1;
    }

    // Print timestamp with nanoseconds
    printf("Timestamp: %s.%09ld\n", time_str, ts.tv_nsec);

    return 0;
}
