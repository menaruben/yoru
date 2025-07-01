#ifndef __YORU_ALLOCATOR_TYPE_H__
#define __YORU_ALLOCATOR_TYPE_H__

#include <stdlib.h>
#include <stddef.h>
#include "yoru_arena_allocator.h"
#include "../functions/yoru_functions.h"

typedef struct Allocator_t
{
  FUNC(void *, alloc, void *, size_t);
  FUNC(void, free, void *);
  void *context;
} Allocator_t;

// arena allocator

static void *arena_alloc(void *context, size_t size)
{
  ArenaAllocator_t *arena = (ArenaAllocator_t *)context;
  return ArenaAllocator_Alloc(arena, size);
}

static inline void arena_free(void *context)
{
  ArenaAllocator_t *arena = (ArenaAllocator_t *)context;
  ArenaAllocator_Free(arena);
}

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
  allocator->free = arena_free;
  return allocator;
}

// heap allocators
static inline void *heap_alloc(void *context, size_t size)
{
  (void)context;
  return malloc(size);
}

static inline void heap_free(void *ptr)
{
  free(ptr);
}

Allocator_t *HeapAllocator_new(void)
{
  Allocator_t *allocator = (Allocator_t *)malloc(sizeof(Allocator_t));
  if (allocator == NULL)
    return NULL;

  allocator->context = NULL;
  allocator->alloc = heap_alloc;
  allocator->free = heap_free;
  return allocator;
}

#endif
