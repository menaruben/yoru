#ifndef __YORU_ALLOCATOR_H__
#define __YORU_ALLOCATOR_H__

/*
  TODOs:
    - [ ] move away from malloc/calloc/realloc/free and implement wrappers for unix, windows and macos instead using mmap or VirtualAlloc etc.
          (only use malloc and so on as fallback):
      - [ ] global alloc.
      - [ ] arena alloc.

    - [ ] add test/debug allocators that log or count allocated bytes

    - [] add chain allocator (basically Linked List of arenas) if you try to push
         but it has no space for that in the arena, then create a new arena and push it onto there
*/

#include "../types/types.h"
#include "../types/optionals.h"

typedef Yoru_Opt (*Yoru_Allocator_Alloc_Func)(anyptr ctx, usize size);
typedef void     (*Yoru_Allocator_DeAlloc_Func)(anyptr ctx, anyptr ptr);
typedef Yoru_Opt (*Yoru_Allocator_ReAlloc_Func)(anyptr ctx, usize old_size, anyptr old_ptr, usize new_size);
typedef void     (*Yoru_Allocator_Destroy_Func)(anyptr ctx);

typedef struct Yoru_AllocatorVTable {
  Yoru_Allocator_Alloc_Func   alloc;
  Yoru_Allocator_DeAlloc_Func dealloc;
  Yoru_Allocator_ReAlloc_Func realloc;
  Yoru_Allocator_Destroy_Func destroy;
} Yoru_AllocatorVTable;

typedef struct Yoru_Allocator {
  const Yoru_AllocatorVTable *vtable;
  anyptr                      ctx;
} Yoru_Allocator;

Yoru_Opt yoru_allocator_alloc(Yoru_Allocator *allocator, usize size);
void     yoru_allocator_dealloc(Yoru_Allocator *allocator, anyptr ptr);
Yoru_Opt yoru_allocator_realloc(Yoru_Allocator *allocator, usize old_size, anyptr old_ptr, usize new_size);
void     yoru_allocator_destroy(Yoru_Allocator *allocator);

#ifdef YORU_IMPL
  #include "../contracts/contracts.h"

Yoru_Opt yoru_allocator_alloc(Yoru_Allocator *allocator, usize size) {
  Yoru_Guard_NotNull(allocator);
  Yoru_Guard_NotNull(allocator->vtable);
  Yoru_Guard_NotNull(allocator->vtable->alloc);
  return allocator->vtable->alloc(allocator->ctx, size);
}

void yoru_allocator_dealloc(Yoru_Allocator *allocator, anyptr ptr) {
  Yoru_Guard_NotNull(allocator);
  Yoru_Guard_NotNull(allocator->vtable);
  Yoru_Guard_NotNull(allocator->vtable->dealloc);
  allocator->vtable->dealloc(allocator->ctx, ptr);
}

Yoru_Opt yoru_allocator_realloc(Yoru_Allocator *allocator, usize old_size, anyptr old_ptr, usize new_size) {
  Yoru_Guard_NotNull(allocator);
  Yoru_Guard_NotNull(allocator->vtable);
  Yoru_Guard_NotNull(allocator->vtable->realloc);
  return allocator->vtable->realloc(allocator->ctx, old_size, old_ptr, new_size);
}

void yoru_allocator_destroy(Yoru_Allocator *allocator) {
  Yoru_Guard_NotNull(allocator);
  Yoru_Guard_NotNull(allocator->vtable);
  Yoru_Guard_NotNull(allocator->vtable->destroy);
  allocator->vtable->destroy(allocator->ctx);
}

#endif
#endif
