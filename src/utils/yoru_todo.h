#ifndef __YORU_TODO_H__
#define __YORU_TODO_H__

#include <stdio.h>
#include <stdlib.h>

#define TODO(msg)                                                          \
    do                                                                     \
    {                                                                      \
        fprintf(stderr, "TODO: '%s' at %s:%d\n", msg, __FILE__, __LINE__); \
        exit(-1);                                                          \
    } while (0)

#endif
