#ifndef __YORU_ARENA_ALLOCATOR_H__
#define __YORU_ARENA_ALLOCATOR_H__

#include <stddef.h>
#include <stdlib.h>

typedef struct
{
  void *memory;
  size_t capacity;
  size_t next_offset;
} ArenaAllocator_t;

ArenaAllocator_t *ArenaAllocator_Init(size_t capacity);
void *ArenaAllocator_Alloc(ArenaAllocator_t *arena, const size_t size);
void ArenaAllocator_Free(ArenaAllocator_t *arena);

ArenaAllocator_t *ArenaAllocator_Init(size_t capacity)
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

void *ArenaAllocator_Alloc(ArenaAllocator_t *arena, const size_t size)
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

void ArenaAllocator_Free(ArenaAllocator_t *arena)
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
