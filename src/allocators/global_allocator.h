#ifndef __YORU_GLOBAL_ALLOCATOR_H__
#define __YORU_GLOBAL_ALLOCATOR_H__

#include "allocator.h"

typedef Yoru_Allocator Yoru_GlobalAllocator;
Yoru_GlobalAllocator   yoru_global_allocator_make();

#ifdef YORU_IMPL
  #include "../types/types.h"
  #include <stdlib.h>
  #include <string.h>
  #include <stddef.h>

Yoru_Opt __yoru_global_allocator_alloc(anyptr ctx, usize size);
void     __yoru_global_allocator_dealloc(anyptr ctx, anyptr ptr);
Yoru_Opt __yoru_global_allocator_realloc(anyptr ctx, usize old_size, anyptr old_ptr, usize new_size);
void     __yoru_global_allocator_destroy(anyptr ctx);

static const Yoru_AllocatorVTable __yoru_global_allocator_vtable = {
    .alloc   = __yoru_global_allocator_alloc,
    .dealloc = __yoru_global_allocator_dealloc,
    .realloc = __yoru_global_allocator_realloc,
    .destroy = __yoru_global_allocator_destroy,
};

Yoru_GlobalAllocator yoru_global_allocator_make() {
  return (Yoru_GlobalAllocator){
      .vtable = &__yoru_global_allocator_vtable,
      .ctx    = NULL,
  };
}

Yoru_Opt __yoru_global_allocator_alloc(anyptr ctx, usize size) {
  (void)ctx;
  anyptr ptr = calloc(1, size);
  if (!ptr) { return yoru_opt_none(); }
  return yoru_opt_some(ptr);
}

void __yoru_global_allocator_dealloc(anyptr ctx, anyptr ptr) {
  (void)ctx;
  if (!ptr) return;
  free(ptr);
}

Yoru_Opt __yoru_global_allocator_realloc(anyptr ctx, usize old_size, anyptr old_ptr, usize new_size) {
  (void)ctx;
  usize  copy_size = old_size < new_size ? old_size : new_size;
  anyptr new_ptr   = calloc(1, new_size);
  if (!new_ptr) return yoru_opt_none();
  memcpy(new_ptr, old_ptr, copy_size);
  return yoru_opt_some(new_ptr);
}

void __yoru_global_allocator_destroy(anyptr ctx) {
  (void)ctx;
}

#endif
#endif
