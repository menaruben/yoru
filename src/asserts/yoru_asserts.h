#ifndef __YORU_ASSERTS_H__
#define __YORU_ASSERTS_H__

#include <stdio.h>

#define ASSERT_EQUAL(a, b)                                                                    \
    if ((a) != (b))                                                                           \
    {                                                                                         \
        fprintf(stderr, "Assertion failed: %s == %s at %s:%d\n", #a, #b, __FILE__, __LINE__); \
        exit(-1);                                                                             \
    }

#define ASSERT_NOT_EQUAL(a, b)                                                                \
    if ((a) == (b))                                                                           \
    {                                                                                         \
        fprintf(stderr, "Assertion failed: %s != %s at %s:%d\n", #a, #b, __FILE__, __LINE__); \
        exit(-1);                                                                             \
    }

#endif