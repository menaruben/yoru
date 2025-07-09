#ifndef __YORU_HEAP_ALLOCATOR_H__
#define __YORU_HEAP_ALLOCATOR_H__

#include <stddef.h>
#include <stdlib.h>
#include "yoru_allocator_type.h"
#include "../yoru_defs.h"

YORU_API Yoru_Allocator_t *HeapAllocator_new(void);

YORU_PRIVATE void *heap_alloc(void *context, size_t size);
YORU_PRIVATE void *heap_realloc(void *ptr, size_t size);
YORU_PRIVATE void heap_free(void *context, void *ptr);

/**
 * @brief Creates and initializes a new heap allocator instance.
 *
 * This function allocates and returns a pointer to a new Yoru_Allocator_t object
 * that manages memory using the heap. The returned allocator can be used
 * for dynamic memory allocation and deallocation.
 *
 * @return Pointer to a newly created Yoru_Allocator_t instance, or NULL on failure.
 */
YORU_API Yoru_Allocator_t *HeapAllocator_new(void)
{
    Yoru_Allocator_t *allocator = (Yoru_Allocator_t *)malloc(sizeof(Yoru_Allocator_t));
    if (allocator == NULL)
        return NULL;

    allocator->context = NULL;
    allocator->alloc = heap_alloc;
    allocator->realloc = heap_realloc;
    allocator->free = heap_free;
    return allocator;
}

YORU_PRIVATE void *heap_alloc(void *context, size_t size)
{
    (void)context;
    return malloc(size);
}

YORU_PRIVATE void *heap_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

YORU_PRIVATE void heap_free(void *context, void *ptr)
{
    (void)context;
    YORU_ASSERT_NOT_NULL(ptr);
    free(ptr);
}

#endif