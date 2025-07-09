#ifndef __YORU_ARENA_ALLOCATOR_H__
#define __YORU_ARENA_ALLOCATOR_H__

#include <stddef.h>
#include <stdlib.h>
#include "yoru_allocator_type.h"
#include "../yoru_defs.h"

typedef struct
{
  void *memory;
  size_t capacity;
  size_t next_offset;
} ArenaAllocator_t;

YORU_API Yoru_Allocator_t *Yoru_ArenaAllocator_new(size_t capacity);

YORU_PRIVATE void *arena_alloc(void *context, size_t size);
YORU_PRIVATE void *arena_realloc(void *ptr, size_t size);
YORU_PRIVATE void arena_free(void *context, void *ptr);

YORU_PRIVATE ArenaAllocator_t *ArenaAllocator_Init(size_t capacity);
YORU_PRIVATE void *ArenaAllocator_Alloc(ArenaAllocator_t *arena, const size_t size);
YORU_PRIVATE void ArenaAllocator_Free(ArenaAllocator_t *arena);

/**
 * @brief Creates a new Arena Allocator with the specified capacity.
 *
 * Allocates and initializes an Arena Allocator, which manages a contiguous block of memory
 * for fast, linear allocation. The allocator does not support freeing individual allocations,
 * but can reset or free the entire arena at once.
 *
 * @param capacity The total size in bytes to allocate for the arena.
 * @return Pointer to the newly created Yoru_Allocator_t instance, or NULL on failure.
 */
YORU_API Yoru_Allocator_t *Yoru_ArenaAllocator_new(size_t capacity)
{
  ArenaAllocator_t *arena = ArenaAllocator_Init(capacity);
  if (arena == NULL)
    return NULL;

  Yoru_Allocator_t *allocator = (Yoru_Allocator_t *)malloc(sizeof(Yoru_Allocator_t));
  if (allocator == NULL)
    return NULL;

  allocator->context = (void *)arena;
  allocator->alloc = arena_alloc;
  allocator->realloc = arena_realloc;
  allocator->free = arena_free;
  return allocator;
}

// Wrappers around the ArenaAllocator functions to fit the Yoru_Allocator_t interface
YORU_PRIVATE void *arena_alloc(void *context, size_t size)
{
  ArenaAllocator_t *arena = (ArenaAllocator_t *)context;
  return ArenaAllocator_Alloc(arena, size);
}

YORU_PRIVATE void *arena_realloc(void *ptr, size_t size)
{
  (void)ptr;
  (void)size;
#ifdef YORU_DEBUG
  YORU_ERROR("Realloc is not supported in ArenaAllocator");
#endif
  return NULL;
}

YORU_PRIVATE void arena_free(void *context, void *ptr)
{
  (void)ptr;
  YORU_ASSERT_NOT_NULL(context);
  ArenaAllocator_t *arena = (ArenaAllocator_t *)context;
  ArenaAllocator_Free(arena);
}

// Implementation of the ArenaAllocator functions

YORU_PRIVATE ArenaAllocator_t *ArenaAllocator_Init(size_t capacity)
{
  ArenaAllocator_t *arena =
      (ArenaAllocator_t *)malloc(sizeof(ArenaAllocator_t));
  if (arena == NULL)
  {
    return NULL;
  }

  arena->memory = malloc(capacity);
  if (arena->memory == NULL)
  {
    free(arena);
    return NULL;
  }

  arena->capacity = capacity;
  arena->next_offset = 0;

  return arena;
}

YORU_PRIVATE void *ArenaAllocator_Alloc(ArenaAllocator_t *arena, const size_t size)
{
  if (arena->memory == NULL || arena->next_offset + size > arena->capacity ||
      arena->capacity == 0)
  {
    return NULL;
  }

  void *ptr = arena->memory + arena->next_offset;
  arena->next_offset += size;
  return ptr;
}

YORU_PRIVATE void ArenaAllocator_Free(ArenaAllocator_t *arena)
{
  if (arena == NULL || arena->memory == NULL)
  {
    return;
  }
  free(arena->memory);
  arena->memory = NULL;
  arena->capacity = 0;
  arena->next_offset = 0;
  free(arena);
}

#endif
