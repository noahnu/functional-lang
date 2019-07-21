#ifndef _H_LOGGING_
#define _H_LOGGING_

#include <stdarg.h>
#include <stdio.h>

#define DEBUG 1

#define DEBUG_PRINT(fmt, ...) \
    do { \
        if (DEBUG) { \
            fprintf(stderr, "%s:%d:%s(): " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
        } \
    } while (0)

#endif