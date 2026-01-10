#ifndef __yoru_virtual_arena_ALLOCATOR_H__
#define __yoru_virtual_arena_ALLOCATOR_H__

#include "allocator.h"
#include "../types/types.h"
#include "vmem.h"

typedef Yoru_Allocator      Yoru_VirtualArenaAllocator;
Yoru_VirtualArenaAllocator *yoru_virtual_arena_allocator_make(usize capacity);

#ifdef YORU_IMPL
#  include <stdlib.h>
#  include <stddef.h>

Yoru_Opt __yoru_virtual_arena_allocator_alloc(anyptr ctx, usize size);
void     __yoru_virtual_arena_allocator_dealloc(anyptr ctx, anyptr ptr);
Yoru_Opt __yoru_virtual_arena_allocator_realloc(anyptr ctx, usize old_size, anyptr old_ptr, usize new_size);
void     __yoru_virtual_arena_allocator_destroy(anyptr ctx);

static const Yoru_AllocatorVTable __yoru_virtual_arena_allocator_vtable = {
    .alloc   = __yoru_virtual_arena_allocator_alloc,
    .dealloc = __yoru_virtual_arena_allocator_dealloc,
    .realloc = __yoru_virtual_arena_allocator_realloc,
    .destroy = __yoru_virtual_arena_allocator_destroy,
};

typedef struct Yoru_VirtualArenaAllocatorCtx {
  usize          offset;
  Yoru_Vmem_Ctx *vmem_ctx;
} Yoru_VirtualArenaAllocatorCtx;

Yoru_ArenaAllocator *yoru_virtual_allocator_make(usize capacity) {
  Yoru_ArenaAllocator *a = calloc(1, sizeof *a);
  if (!a) return NULL;

  Yoru_VirtualArenaAllocatorCtx *ctx = calloc(1, sizeof *ctx);
  if (!ctx) goto err;

  Yoru_Vmem_Ctx *vmem_ctx = calloc(1, sizeof *vmem_ctx);
  if (!vmem_ctx) goto err;

  capacity = yoru_align_up(capacity, yoru_get_page_size());

  if (!yoru_vmem_reserve(capacity, vmem_ctx)) goto err;
  if (!yoru_vmem_commit(vmem_ctx, yoru_get_page_size())) goto err;

  ctx->offset   = 0;
  ctx->vmem_ctx = vmem_ctx;

  a->vtable = &__yoru_virtual_arena_allocator_vtable;
  a->ctx    = ctx;
  return a;

err:
  if (vmem_ctx) {
    yoru_vmem_free(vmem_ctx);
    free(vmem_ctx);
  }
  free(ctx);
  free(a);
  return NULL;
}

Yoru_Opt __yoru_virtual_arena_allocator_alloc(anyptr ctx, usize size) {
  if (!ctx) return yoru_opt_none();

  Yoru_VirtualArenaAllocatorCtx *arena = ctx;
  Yoru_Vmem_Ctx                 *vm    = arena->vmem_ctx;
  if (arena->offset + size > vm->addr_space_size) return yoru_opt_none();

  usize needed = arena->offset + size;
  usize page   = yoru_get_page_size();

  while (vm->commit_pos < needed) {
    if (!yoru_vmem_commit(vm, page)) return yoru_opt_none();
  }

  anyptr ptr = (char *)vm->base + arena->offset;
  arena->offset += size;

  return yoru_opt_some(ptr);
}

void __yoru_virtual_arena_allocator_dealloc(anyptr ctx, anyptr ptr) {
  (void)ctx;
  (void)ptr;
  /* Freeing in an arena is a no-op because the idea is that you free the entire arena once.
     For that you would call the yoru_allocator_destroy func on the arena allocator */
}

Yoru_Opt __yoru_virtual_arena_allocator_realloc(anyptr ctx, usize old_size, anyptr old_ptr, usize new_size) {
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

void __yoru_virtual_arena_allocator_destroy(anyptr ctx) {
  assert(ctx && "must not be null");
  Yoru_VirtualArenaAllocatorCtx *c = (Yoru_VirtualArenaAllocatorCtx *)ctx;
  assert(c->vmem_ctx && "must not be null");

  yoru_vmem_free(c->vmem_ctx);
  free(c->vmem_ctx);
  free(c);
  c->offset                    = 0;
  c->vmem_ctx->base            = NULL;
  c->vmem_ctx->commit_pos      = 0;
  c->vmem_ctx->addr_space_size = 0;
}

#endif
#endif
