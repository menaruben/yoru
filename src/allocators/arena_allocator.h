#ifndef __YORU_ARENA_ALLOCATOR_H__
#define __YORU_ARENA_ALLOCATOR_H__

#include "allocator.h"
#include "../types/types.h"

typedef Yoru_Allocator Yoru_ArenaAllocator;
Yoru_ArenaAllocator   *yoru_arena_allocator_make(usize capacity);

#ifdef YORU_IMPL
  #include <stdlib.h>
  #include <stddef.h>

Yoru_Opt __yoru_arena_allocator_alloc(anyptr ctx, usize size);
void     __yoru_arena_allocator_dealloc(anyptr ctx, anyptr ptr);
Yoru_Opt __yoru_arena_allocator_realloc(anyptr ctx, usize old_size, anyptr old_ptr, usize new_size);
void     __yoru_arena_allocator_destroy(anyptr ctx);

static const Yoru_AllocatorVTable __yoru_arena_allocator_vtable = {
    .alloc   = __yoru_arena_allocator_alloc,
    .dealloc = __yoru_arena_allocator_dealloc,
    .realloc = __yoru_arena_allocator_realloc,
    .destroy = __yoru_arena_allocator_destroy,
};

typedef struct Yoru_ArenaAllocatorCtx {
  byte *mem;
  usize offset;
  usize capacity;
} Yoru_ArenaAllocatorCtx;

Yoru_ArenaAllocator *yoru_arena_allocator_make(usize capacity) {
  Yoru_ArenaAllocator    *a   = NULL;
  Yoru_ArenaAllocatorCtx *ctx = NULL;
  byte                   *mem = NULL;

  a = calloc(1, sizeof(Yoru_ArenaAllocator));
  if (!a) goto yoru_arena_allocator_make_err;
  a->vtable = &__yoru_arena_allocator_vtable;

  ctx = calloc(1, sizeof(Yoru_ArenaAllocatorCtx));
  if (!ctx) goto yoru_arena_allocator_make_err;
  a->ctx = (anyptr)ctx;

  mem = (byte *)calloc(1, capacity);
  if (!mem) goto yoru_arena_allocator_make_err;
  ctx->mem      = mem;
  ctx->offset   = 0;
  ctx->capacity = capacity;
  return a;

yoru_arena_allocator_make_err:
  if (a) free(a);
  if (ctx) free(ctx);
  if (mem) free(mem);

  return NULL;
}

Yoru_Opt __yoru_arena_allocator_alloc(anyptr ctx, usize size) {
  if (!ctx) return yoru_opt_none();
  Yoru_ArenaAllocatorCtx *arena = (Yoru_ArenaAllocatorCtx *)ctx;

  if (arena->offset + size > arena->capacity) { return yoru_opt_none(); }

  anyptr ptr = arena->mem + arena->offset;
  arena->offset += size;

  return yoru_opt_some(ptr);
}

void __yoru_arena_allocator_dealloc(anyptr ctx, anyptr ptr) {
  (void)ctx;
  (void)ptr;
  /* Freeing in an arena is a no-op because the idea is that you free the entire arena once.
     For that you would call the yoru_allocator_destroy func on the arena allocator */
}

Yoru_Opt __yoru_arena_allocator_realloc(anyptr ctx, usize old_size, anyptr old_ptr, usize new_size) {
  (void)ctx;
  (void)old_size;
  (void)old_ptr;
  (void)new_size;
  /* Reallocations are not supported on arena allocators because if you want to grow something in the
     middle of the arena then we would also have to shift every ptr to the right which might not work
     becuase the arena might not be able to fit everything with the new ptr in the arena and we dont
     know in the arena ctx where which ptr starts! otherwise we would have to keep track of them here.

     -> maybe we could handle reallocs like normal allocations instead and just push to the arena?
  */
  return yoru_opt_none();
}

void __yoru_arena_allocator_destroy(anyptr ctx) {
  if (!ctx) return;
  Yoru_ArenaAllocatorCtx *c = (Yoru_ArenaAllocatorCtx *)ctx;
  if (c->mem) free(c->mem);
  c->mem      = NULL;
  c->offset   = 0;
  c->capacity = 0;
}

#endif
#endif
