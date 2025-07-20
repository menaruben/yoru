#ifndef __YORU_SLICE_H__
#define __YORU_SLICE_H__

#include "../yoru_defs.h"
#include "../results/yoru_results.h"
#include <stdlib.h>

/// @brief Represents a slice of memory with an offset (where to write new data) and a capacity (total size of the allocated memory).
typedef struct Yoru_Slice_t
{
    void *data;
    size_t offset;   // offset in bytes from the start of the allocated memory (for heap always 0)
    size_t capacity; // total size of the allocated memory in bytes
} Yoru_Slice_t;

#endif