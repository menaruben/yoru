#ifndef __YORU_ARENA_ALLOCATOR_H__
#define __YORU_ARENA_ALLOCATOR_H__

#include <stddef.h>
#include <stdlib.h>
#include "yoru_allocator_type.h"
#include "../yoru_defs.h"
#include "../results/yoru_results.h"

typedef struct
{
  void *memory;
  size_t capacity;
  size_t next_offset;
} Yoru_ArenaAllocator_t;

YORU_API Yoru_Allocator_t *Yoru_ArenaAllocator_new(size_t capacity);

YORU_PRIVATE void *yoru_arena_alloc(void *context, size_t size);
YORU_PRIVATE Yoru_Result_t yoru_arena_alloc_try(void *context, size_t size);

YORU_PRIVATE void *yoru_arena_realloc(void *ptr, size_t size);
YORU_PRIVATE Yoru_Result_t yoru_arena_realloc_try(void *context, void *ptr, size_t size);

YORU_PRIVATE void yoru_arena_free(void *context, void *ptr);
YORU_PRIVATE Yoru_Result_t yoru_arena_free_try(void *context, void *ptr);

YORU_PRIVATE Yoru_ArenaAllocator_t *Yoru_ArenaAllocator_Init(size_t capacity);
YORU_PRIVATE void *ArenaAllocator_Alloc(Yoru_ArenaAllocator_t *arena, const size_t size);
YORU_PRIVATE void ArenaAllocator_Free(Yoru_ArenaAllocator_t *arena);

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
  Yoru_ArenaAllocator_t *arena = Yoru_ArenaAllocator_Init(capacity);
  if (arena == NULL)
    return NULL;

  Yoru_Allocator_t *allocator = (Yoru_Allocator_t *)malloc(sizeof(Yoru_Allocator_t));
  if (allocator == NULL)
    return NULL;

  allocator->context = (void *)arena;
  allocator->alloc = yoru_arena_alloc;
  allocator->alloc_try = yoru_arena_alloc_try;
  allocator->realloc = yoru_arena_realloc;
  allocator->realloc_try = yoru_arena_realloc_try;
  allocator->free = yoru_arena_free;
  allocator->free_try = yoru_arena_free_try;
  return allocator;
}

// Wrappers around the ArenaAllocator functions to fit the Yoru_Allocator_t interface
YORU_PRIVATE void *yoru_arena_alloc(void *context, size_t size)
{
  Yoru_ArenaAllocator_t *arena = (Yoru_ArenaAllocator_t *)context;
  return ArenaAllocator_Alloc(arena, size);
}

YORU_PRIVATE Yoru_Result_t yoru_arena_alloc_try(void *context, size_t size)
{
  void *ptr = yoru_arena_alloc(context, size);
  if (ptr == NULL)
  {
    return (Yoru_Result_t){NULL, YORU_ERR_ALLOC};
  }
  return (Yoru_Result_t){ptr, YORU_OK};
}

YORU_PRIVATE void *yoru_arena_realloc(void *ptr, size_t size)
{
  (void)ptr;
  (void)size;
#ifdef YORU_EXIT_ON_ARENA_REALLOC
  YORU_ERROR("Realloc is not supported in ArenaAllocator");
#endif
  return NULL;
}

YORU_PRIVATE Yoru_Result_t yoru_arena_realloc_try(void *context, void *ptr, size_t size)
{
  (void)ptr;
  (void)size;
  if (context)
  {
    return (Yoru_Result_t){NULL, YORU_ERR_REALLOC};
  }
  return (Yoru_Result_t){NULL, YORU_ERR_ARGUMENT_NULL};
}

YORU_PRIVATE void yoru_arena_free(void *context, void *ptr)
{
  (void)ptr;
  if (context)
  {
    Yoru_ArenaAllocator_t *arena = (Yoru_ArenaAllocator_t *)context;
    ArenaAllocator_Free(arena);
  }
}

YORU_PRIVATE Yoru_Result_t yoru_arena_free_try(void *context, void *ptr)
{
  (void)ptr;
  if (context)
  {
    yoru_arena_free(context, ptr);
    return (Yoru_Result_t){NULL, YORU_OK};
  }
  return (Yoru_Result_t){NULL, YORU_ERR_ARGUMENT_NULL};
}

// Implementation of the ArenaAllocator functions

YORU_PRIVATE Yoru_ArenaAllocator_t *Yoru_ArenaAllocator_Init(size_t capacity)
{
  Yoru_ArenaAllocator_t *arena =
      (Yoru_ArenaAllocator_t *)malloc(sizeof(Yoru_ArenaAllocator_t));
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

YORU_PRIVATE void *ArenaAllocator_Alloc(Yoru_ArenaAllocator_t *arena, const size_t size)
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

YORU_PRIVATE void ArenaAllocator_Free(Yoru_ArenaAllocator_t *arena)
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
