#ifndef __YORU_HEAP_H__
#define __YORU_HEAP_H__

#include "yoru_slice.h"
#include "../yoru_defs.h"
#include "../results/yoru_results.h"
#include "../utils/yoru_utils.h"
#include <stdlib.h>

YORU_API Yoru_Allocator_Result_t yoru_heap_alloc(size_t capacity);

YORU_API Yoru_Allocator_Result_t yoru_heap_realloc(Yoru_Slice_t *slice, size_t capacity);

YORU_API Yoru_Error_t yoru_heap_free(Yoru_Slice_t *slice);

#ifdef YORU_IMPLEMENTATION

YORU_API Yoru_Allocator_Result_t yoru_heap_alloc(size_t capacity)
{
    if (capacity <= 0)
    {
        return Yoru_Allocator_Failure(YORU_ERR_ARGUMENT_INVALID, "Capacity must be greater than 0");
    }

    void *mem = malloc(capacity);
    if (!mem)
    {
        return Yoru_Allocator_Failure(YORU_ERR_ALLOC, "Failed to allocate memory");
    }

    return Yoru_Allocator_Success(mem, capacity);
}

YORU_API Yoru_Allocator_Result_t yoru_heap_realloc(Yoru_Slice_t *slice, size_t capacity)
{
    if (capacity <= 0)
    {
        return Yoru_Allocator_Failure(YORU_ERR_ARGUMENT_INVALID, YORU_NAMEOF(capacity) " must be greater than 0");
    }

    if (!slice)
    {
        return Yoru_Allocator_Failure(YORU_ERR_ARGUMENT_NULL, YORU_NAMEOF(slice) " cannot be NULL");
    }

    if (!slice->data)
    {
        return Yoru_Allocator_Failure(YORU_ERR_ARGUMENT_NULL, "Slice data cannot be NULL");
    }

    void *new_mem = realloc(slice->data, capacity);
    if (!new_mem)
    {
        return Yoru_Allocator_Failure(YORU_ERR_REALLOC, "Failed to reallocate memory");
    }

    slice->data = new_mem;
    slice->capacity = capacity;
    return Yoru_Allocator_Success(slice, capacity);
}

YORU_API Yoru_Error_t yoru_heap_free(Yoru_Slice_t *slice)
{
    if (!slice)
    {
        return YORU_ERR_ARGUMENT_NULL;
    }

    if (!slice->data)
    {
        return YORU_ERR_ARGUMENT_NULL;
    }

    free(slice->data);
    slice->data = NULL;
    slice->offset = 0;
    slice->capacity = 0;

    return YORU_OK;
}

#endif

#endif