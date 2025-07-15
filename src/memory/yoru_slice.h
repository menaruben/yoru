#ifndef __YORU_SLICE_H__
#define __YORU_SLICE_H__

#include "../results/yoru_results.h"
#include <stdlib.h>

typedef struct Yoru_Slice_t
{
    void *data;
    size_t offset; // indicates the index where the next write/allocation can be done..
    size_t capacity;
} Yoru_Slice_t;

typedef Yoru_Result(Yoru_Slice_t, Yoru_Error_t) Yoru_Allocator_Result_t;

#define Yoru_Allocator_Success(ptr, size) \
    (Yoru_Allocator_Result_t) { .value = {.data = ptr, .offset = 0, .capacity = size}, .err = YORU_OK, .message = NULL }

#define Yoru_Allocator_Failure(e, msg) \
    (Yoru_Allocator_Result_t) { .value = NULL, .err = e, .message = msg }

#endif