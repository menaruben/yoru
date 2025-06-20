#ifndef __YORU_ARENA_ALLOCATOR_H__
#define __YORU_ARENA_ALLOCATOR_H__

#include <stddef.h>
#include <stdlib.h>

/// Arena Allocator
/// An arena allocator is a memory allocator that allocates a large block of memory at once
/// and then allows for allocations from that block without needing to call malloc repeatedly.
typedef struct
{
    void *memory;
    size_t capacity;
    size_t next_offset;
} ArenaAllocator_t;

/// AllocationResult_t
/// A structure to hold the result of an allocation attempt with the fields:
/// - `ptr`: Pointer to the allocated memory (or NULL if allocation failed).
/// - `size`: The size of the allocated memory (0 if allocation failed).
/// - `error`: A string describing the error (NULL if allocation succeeded).
typedef struct
{
    void *ptr;
    size_t size;
    const char *error;
} AllocationResult_t;

static inline AllocationResult_t *AllocationResult_Success(void *ptr, size_t size)
{
    static AllocationResult_t result;
    result.ptr = ptr;
    result.size = size;
    result.error = NULL;
    return &result;
}

static inline AllocationResult_t *AllocationResult_Failure(const char *error)
{
    static AllocationResult_t result;
    result.ptr = NULL;
    result.size = 0;
    result.error = error;
    return &result;
}

/// @brief
/// @param capacity
/// @return `ArenaAllocator_t`
ArenaAllocator_t ArenaAllocator_Init(size_t capacity)
{
    ArenaAllocator_t allocator;
    allocator.memory = malloc(capacity);
    allocator.capacity = capacity;
    allocator.next_offset = 0;
    return allocator;
}

/// @brief ArenaAllocator_Allocate allocates memory on the given arena allocator.
/// @param allocator is the arena allocator to allocate memory from.
/// @param size which is the size of the memory to allocate.
/// @return `AllocationResult_t*` containing the result of the allocation.
AllocationResult_t *ArenaAllocator_Allocate(ArenaAllocator_t *allocator, size_t size)
{
    if (allocator->next_offset + size > allocator->capacity)
    {
        return AllocationResult_Failure("ArenaAllocator: Out of memory");
    }

    void *ptr = (char *)allocator->memory + allocator->next_offset;
    allocator->next_offset += size;
    return AllocationResult_Success(ptr, size);
}

/// @brief ArenaAllocator_Reset resets the arena allocator, allowing for reuse of the memory.
void ArenaAllocator_Reset(ArenaAllocator_t *allocator)
{
    allocator->next_offset = 0;
}
/*
    TODO:
    Think about how to add support for reallocating memory in the arena allocator.

    Otherwise ArrayLists will not be able to grow dynamically because we copy the old data to the new memory
    and remove the old memory. This creates a sort of "hole" in the memry that is not reused because the next_offset
    is past the index of the old memory.
    - Shifting all data after the old memory is a possibility, but it would be inefficient.
*/

/// @brief ArenaAllocator_Free frees the memory allocated by the arena allocator.
/// @param allocator is the arena allocator to free memory from.
void ArenaAllocator_Free(ArenaAllocator_t *allocator)
{
    if (allocator->memory)
    {
        free(allocator->memory);
        allocator->memory = NULL;
    }
    allocator->capacity = 0;
    allocator->next_offset = 0;
}

#endif