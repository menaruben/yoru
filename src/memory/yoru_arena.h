#ifndef __YORU_ARENA_H__
#define __YORU_ARENA_H__

#include "yoru_slice.h"
#include "../yoru_defs.h"
#include "../utils/yoru_utils.h"
#include <stdlib.h>

/// @brief Allocates a new arena with the specified capacity.
/// @param capacity
/// @return A Yoru_Allocator_Result_t containing the allocated arena slice or an error.
YORU_API Yoru_Allocator_Result_t yoru_arena_alloc(size_t capacity);

/// @brief Allocates a slice from the arena.
/// @param arena
/// @param slice_size
/// @return A Yoru_Allocator_Result_t containing the allocated slice or an error.
YORU_API Yoru_Allocator_Result_t yoru_arena_alloc_slice(Yoru_Slice_t *arena, size_t slice_size);

YORU_API void yoru_arena_free(Yoru_Slice_t *arena);

#ifdef YORU_IMPLEMENTATION

YORU_API Yoru_Allocator_Result_t yoru_arena_alloc(size_t capacity)
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

YORU_API Yoru_Allocator_Result_t yoru_arena_alloc_slice(Yoru_Slice_t *arena, size_t slice_size)
{
    if (!arena)
    {
        return Yoru_Allocator_Failure(YORU_ERR_ARGUMENT_NULL, YORU_NAMEOF(arena) " cannot be NULL");
    }

    if (slice_size <= 0)
    {
        return Yoru_Allocator_Failure(YORU_ERR_ARGUMENT_INVALID, YORU_NAMEOF(slice_size) " must be greater than 0");
    }

    if (!arena->data)
    {
        return Yoru_Allocator_Failure(YORU_ERR_ARGUMENT_NULL, "Arena data cannot be NULL");
    }

    if (arena->offset + slice_size > arena->capacity)
    {
        return Yoru_Allocator_Failure(YORU_ERR_OUT_OF_MEMORY, "Not enough space in arena for slice allocation");
    }

    void *ptr = (char *)arena->data + arena->offset;
    arena->offset += slice_size;
    return Yoru_Allocator_Success(ptr, slice_size);
}

YORU_API void yoru_arena_free(Yoru_Slice_t *arena)
{
    if (arena && arena->data)
    {
        free(arena->data);
        arena->data = NULL;
        arena->offset = 0;
        arena->capacity = 0;
    }
}

#endif

#endif
