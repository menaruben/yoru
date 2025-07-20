#ifndef __YORU_ARENA_H__
#define __YORU_ARENA_H__

#include "yoru_slice.h"
#include "../yoru_defs.h"
#include "../utils/yoru_utils.h"
#include <stdlib.h>

/// @brief Allocates a new arena slice.
/// @param capacity The size of the arena to allocate in bytes.
/// @param[out] arena The output arena slice that will hold the allocated memory.
/// @return Yoru_Error_t
YORU_API Yoru_Error_t yoru_arena_alloc(size_t capacity, YORU_OUT Yoru_Slice_t *arena);

/// @brief Allocates a slice from the arena.
/// @param arena The arena to allocate from.
/// @param slice_size The size of the slice to allocate.
/// @param[out] slice The output slice on the arena.
/// @return Yoru_Error_t
YORU_API Yoru_Error_t yoru_arena_alloc_slice(Yoru_Slice_t *arena, size_t slice_size, YORU_OUT Yoru_Slice_t *slice);

/// @brief Frees the arena slice if it was allocated.
/// @param arena The arena to free.
/// @return void
YORU_API void yoru_arena_free(Yoru_Slice_t *arena);

#ifdef YORU_IMPLEMENTATION

YORU_API Yoru_Error_t yoru_arena_alloc(size_t capacity, YORU_OUT Yoru_Slice_t *arena)
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

    arena->data = mem;
    arena->offset = 0;
    arena->capacity = capacity;
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
}

YORU_API Yoru_Error_t yoru_arena_alloc_slice(Yoru_Slice_t *arena, size_t slice_size, YORU_OUT Yoru_Slice_t *slice)
{
    if (!arena)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = YORU_NAMEOF(arena) " cannot be NULL"};
    }

    if (slice_size <= 0)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_INVALID, .message = YORU_NAMEOF(slice_size) " must be greater than 0"};
    }

    if (!arena->data)
    {
        return (Yoru_Error_t){.type = YORU_ERR_ARGUMENT_NULL, .message = "Arena data cannot be NULL"};
    }

    if (arena->offset + slice_size > arena->capacity)
    {
        return (Yoru_Error_t){.type = YORU_ERR_OUT_OF_MEMORY, .message = "Not enough space in arena for slice allocation"};
    }

    void *ptr = (char *)arena->data + arena->offset;
    arena->offset += slice_size;
    slice->data = ptr;
    slice->offset = 0;
    slice->capacity = slice_size;
    return (Yoru_Error_t){.type = YORU_OK, .message = NULL};
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
