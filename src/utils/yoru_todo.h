#ifndef __YORU_TODO_H__
#define __YORU_TODO_H__

#include <stdio.h>
#include <stdlib.h>

#define YORU_TODO(msg)                                                     \
    do                                                                     \
    {                                                                      \
        fprintf(stderr, "TODO: '%s' at %s:%d\n", msg, __FILE__, __LINE__); \
        exit(-1);                                                          \
    } while (0)

#define YORU_NOT_IMPLEMENTED(msg)                                                     \
    do                                                                                \
    {                                                                                 \
        fprintf(stderr, "NOT IMPLEMENTED: '%s' at %s:%d\n", msg, __FILE__, __LINE__); \
        exit(-1);                                                                     \
    } while (0)

#define YORU_NOT_SUPPORTED(msg)                                                     \
    do                                                                              \
    {                                                                               \
        fprintf(stderr, "NOT SUPPORTED: '%s' at %s:%d\n", msg, __FILE__, __LINE__); \
        exit(-1);                                                                   \
    } while (0)

#endif
