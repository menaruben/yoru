#ifndef __YORU_DYNARRAY_H__
#define __YORU_DYNARRAY_H__

/*
  TODO:
  - [ ] add yoru_dynarray_grow or yoru_dynarray_resize to specifically set capacity
  - [ ] find a better name than dynarray?
    - Vector      : to mathy, might want to use this in yoru math lib
    - ArrayList   : could work but to java-ish, want to keep C-like feel
    - DynamicArray: explicit, but a bit long
    -> for not its f ine but might consider renaming
*/

#include "../types/types.h"
#include "../allocators/allocator.h"

#include <stdbool.h>

#define YORU_DYNARRAY_INITIAL_CAPACITY 16

#define Yoru_Implement_DynArray(__T)                                                                                   \
  __T            *__da_items__;                                                                                        \
  usize           __da_size__, __da_capacity__;                                                                        \
  Yoru_Allocator *__da_allocator__;

#define yoru_dynarray_init(__da_ptr, __allocator_ptr)                                                                  \
  __yoru_dynarray_init_impl(                                                                                           \
      __allocator_ptr,                                                                                                 \
      &((__da_ptr)->__da_allocator__),                                                                                 \
      (anyptr *)&((__da_ptr)->__da_items__),                                                                           \
      &((__da_ptr)->__da_size__),                                                                                      \
      &((__da_ptr)->__da_capacity__),                                                                                  \
      sizeof((__da_ptr)->__da_items__[0]))

#define yoru_dynarray_append(__da_ptr, __value)                                                                        \
  __yoru_dynarray_append_impl(                                                                                         \
      ((__da_ptr)->__da_allocator__),                                                                                  \
      (anyptr *)&((__da_ptr)->__da_items__),                                                                           \
      &((__da_ptr)->__da_size__),                                                                                      \
      &((__da_ptr)->__da_capacity__),                                                                                  \
      sizeof((__da_ptr)->__da_items__[0]),                                                                             \
      &__value)

bool __yoru_dynarray_init_impl(
    Yoru_Allocator  *allocator,
    Yoru_Allocator **pallocator,
    void           **pitems,
    usize           *psize,
    usize           *pcapacity,
    usize            item_size);

bool __yoru_dynarray_append_impl(
    Yoru_Allocator *pallocator, anyptr *pitems, usize *psize, usize *pcapacity, usize item_size, anyptr value);

#ifdef YORU_IMPL
  #include "../contracts/contracts.h"
  #include <string.h>

bool __yoru_dynarray_append_impl(
    Yoru_Allocator *pallocator, anyptr *pitems, usize *psize, usize *pcapacity, usize item_size, anyptr value) {
  Yoru_Guard_NotNull(pallocator);
  Yoru_Guard_NotNull(pitems);
  Yoru_Guard_NotNull(psize);
  Yoru_Guard_NotNull(pcapacity);

  // assume dynamic array was destroyed
  if (!*pitems || *pcapacity == 0) return false;

  if ((*psize) >= *pcapacity) {
    usize new_capacity = *pcapacity;
    // TODO can prob calculate this with log + ceil instead of looping
    while ((*psize) >= new_capacity)
      new_capacity *= 2;

    Yoru_Opt new_items = yoru_allocator_realloc(pallocator, *pcapacity, *pitems, new_capacity);
    if (!new_items.has_value) return false;
    anyptr old_items = *pitems;
    *pitems          = new_items.ptr;
    *pcapacity       = new_capacity;
    yoru_allocator_free(pallocator, old_items);
  }

  memcpy(*pitems + *psize * item_size, value, item_size);
  *psize += 1;
  return true;
}

bool __yoru_dynarray_init_impl(
    Yoru_Allocator  *allocator,
    Yoru_Allocator **pallocator,
    void           **pitems,
    usize           *psize,
    usize           *pcapacity,
    usize            item_size) {
  Yoru_Guard_NotNull(allocator);
  Yoru_Guard_NotNull(pallocator);
  Yoru_Guard_NotNull(pitems);
  Yoru_Guard_NotNull(psize);
  Yoru_Guard_NotNull(pcapacity);

  usize capacity = *pcapacity;
  if (capacity == 0) capacity = YORU_DYNARRAY_INITIAL_CAPACITY;

  Yoru_Opt maybe_items = yoru_allocator_alloc(allocator, item_size * capacity);
  if (!maybe_items.has_value) goto yoru_dynarray_init_err;
  anyptr items = maybe_items.ptr;

  *psize      = 0;
  *pcapacity  = capacity;
  *pitems     = items;
  *pallocator = allocator;
  return true;

yoru_dynarray_init_err:
  if (!items) yoru_allocator_free(allocator, items);
  return false;
}

#endif
#endif // __YORU_DYNARRAY_H__
