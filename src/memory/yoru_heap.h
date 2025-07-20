#ifndef __YORU_HEAP_H__
#define __YORU_HEAP_H__

#include "yoru_slice.h"
#include "../yoru_defs.h"
#include "../results/yoru_results.h"
#include "../utils/yoru_utils.h"
#include <stdlib.h>

/// @brief Allocates a slice of memory on the heap.
/// @param capacity The size of the memory to allocate in bytes.
/// @param[out] slice The output slice that will hold the allocated memory.
/// @return Yoru_Error_t
YORU_API Yoru_Error_t yoru_heap_alloc(size_t capacity, YORU_OUT Yoru_Slice_t *slice);

/// @brief Reallocates a slice of memory on the heap.
/// @param capacity The new size of the memory to allocate in bytes.
/// @param slice The slice to reallocate.
/// @return Yoru_Error_t
YORU_API Yoru_Error_t yoru_heap_realloc(size_t capacity, Yoru_Slice_t *slice);

/// @brief Frees a slice of memory allocated on the heap.
/// @param slice The slice to free. If the slice is NULL or its data is NULL
/// the function does nothing.
YORU_API void yoru_heap_free(Yoru_Slice_t *slice);

#ifdef YORU_IMPLEMENTATION

YORU_API Yoru_Error_t yoru_heap_alloc(size_t capacity, YORU_OUT Yoru_Slice_t *slice)
{
    if (capacity <= 0)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_INVALID, .message = YORU_NAMEOF(capacity) " must be greater than 0"};
    }

    void *mem = malloc(capacity);
    if (!mem)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ALLOC, .message = "Failed to allocate memory"};
    }

    slice->data = mem;
    slice->offset = 0;
    slice->capacity = capacity;
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API Yoru_Error_t yoru_heap_realloc(size_t capacity, Yoru_Slice_t *slice)
{
    if (capacity <= 0)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_INVALID, .message = YORU_NAMEOF(capacity) " must be greater than 0"};
    }

    if (!slice)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(slice) " cannot be NULL"};
    }

    if (!slice->data)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(slice->data) " cannot be NULL"};
    }

    void *new_mem = realloc(slice->data, capacity);
    if (!new_mem)
    {
        return (Yoru_Error_t){.type = YORU_ERR_REALLOC, .message = "Failed to reallocate memory"};
    }

    slice->data = new_mem;
    slice->capacity = capacity;
    if (slice->offset > capacity)
    {
        slice->offset = capacity;
    }
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API void yoru_heap_free(Yoru_Slice_t *slice)
{
    if (!slice || !slice->data)
        return;

    free(slice->data);
    slice->data = NULL;
    slice->offset = 0;
    slice->capacity = 0;
}

#endif

#endif