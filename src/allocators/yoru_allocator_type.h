#ifndef __YORU_ALLOCATOR_TYPE_H__
#define __YORU_ALLOCATOR_TYPE_H__

#include <stdlib.h>
#include <stddef.h>
#include "yoru_arena_allocator.h"
#include "../functions/yoru_functions.h"
#include "../asserts/yoru_asserts.h"

/**
 * @brief Structure representing a memory allocator.
 *
 * This struct defines the interface and data required for a custom memory allocator.
 * It typically contains function pointers for allocation, deallocation, and possibly
 * other memory management operations, as well as any state or configuration data
 * needed by the allocator.
 * * The `context` field can be used to store any additional data needed by the allocator,
 *   such as a pointer to an arena or other allocator-specific state.
 */
typedef struct Allocator_t
{
  FUNC(void *, alloc, void *, size_t);
  FUNC(void, free, void *, void *);
  FUNC(void *, realloc, void *, size_t);
  void *context;
} Allocator_t;

// arena allocator
static void *arena_alloc(void *context, size_t size)
{
  ArenaAllocator_t *arena = (ArenaAllocator_t *)context;
  return ArenaAllocator_Alloc(arena, size);
}

static void *arena_realloc(void *ptr, size_t size)
{
  (void)ptr;
  (void)size;
  ERROR("Realloc is not supported in ArenaAllocator");
  return NULL;
}

static inline void arena_free(void *context, void *ptr)
{
  (void)ptr;
  ASSERT_NOT_NULL(context);
  ArenaAllocator_t *arena = (ArenaAllocator_t *)context;
  ArenaAllocator_Free(arena);
}

/**
 * @brief Creates a new Arena Allocator with the specified capacity.
 *
 * Allocates and initializes an Arena Allocator, which manages a contiguous block of memory
 * for fast, linear allocation. The allocator does not support freeing individual allocations,
 * but can reset or free the entire arena at once.
 *
 * @param capacity The total size in bytes to allocate for the arena.
 * @return Pointer to the newly created Allocator_t instance, or NULL on failure.
 */
Allocator_t *ArenaAllocator_new(size_t capacity)
{
  ArenaAllocator_t *arena = ArenaAllocator_Init(capacity);
  if (arena == NULL)
    return NULL;

  Allocator_t *allocator = (Allocator_t *)malloc(sizeof(Allocator_t));
  if (allocator == NULL)
    return NULL;

  allocator->context = (void *)arena;
  allocator->alloc = arena_alloc;
  allocator->realloc = arena_realloc;
  allocator->free = arena_free;
  return allocator;
}

// heap allocators
static inline void *heap_alloc(void *context, size_t size)
{
  (void)context;
  return malloc(size);
}

static inline void *heap_realloc(void *ptr, size_t size)
{
  return realloc(ptr, size);
}

static inline void heap_free(void *context, void *ptr)
{
  (void)context;
  ASSERT_NOT_NULL(ptr);
  free(ptr);
}

/**
 * @brief Creates and initializes a new heap allocator instance.
 *
 * This function allocates and returns a pointer to a new Allocator_t object
 * that manages memory using the heap. The returned allocator can be used
 * for dynamic memory allocation and deallocation.
 *
 * @return Pointer to a newly created Allocator_t instance, or NULL on failure.
 */
Allocator_t *HeapAllocator_new(void)
{
  Allocator_t *allocator = (Allocator_t *)malloc(sizeof(Allocator_t));
  if (allocator == NULL)
    return NULL;

  allocator->context = NULL;
  allocator->alloc = heap_alloc;
  allocator->realloc = heap_realloc;
  allocator->free = heap_free;
  return allocator;
}

#endif
