#ifndef __YORU_DYNARRAY_H__
#define __YORU_DYNARRAY_H__

#include <assert.h>
#include "../types/types.h"
#include "../allocators/allocator.h"
#include "../contracts/contracts.h"
#include "../types/optionals.h"
#include <string.h>

#define YORU_ARRAYLIST_INITIAL_CAPACITY (16)

/*
  TODO: figure out how i can handle errors better here
*/

#define Yoru_ArrayList_T(__T)                                                                                          \
  struct {                                                                                                             \
    __T            *items;                                                                                             \
    usize           size, capacity;                                                                                    \
    Yoru_Allocator *allocator;                                                                                         \
  }

#define yoru_arraylist_init(__arr_ptr, __allocator_ptr, __capacity)                                                    \
  do {                                                                                                                 \
    Yoru_Opt maybe_items =                                                                                             \
        yoru_allocator_alloc((__allocator_ptr), YORU_ARRAYLIST_INITIAL_CAPACITY * sizeof((__arr_ptr)->items[0]));      \
    assert(maybe_items.has_value && "could not allocate memory for arraylist");                                        \
    (__arr_ptr)->items     = maybe_items.ptr;                                                                          \
    (__arr_ptr)->size      = 0;                                                                                        \
    (__arr_ptr)->capacity  = ((__capacity) == 0) ? YORU_ARRAYLIST_INITIAL_CAPACITY : (__capacity);                     \
    (__arr_ptr)->allocator = __allocator_ptr;                                                                          \
  } while (0);

#define yoru_arraylist_append(__arr_ptr, __value)                                                                      \
  do {                                                                                                                 \
    Yoru_Guard_NotNull((__arr_ptr));                                                                                   \
    if ((__arr_ptr)->size + 1 > (__arr_ptr)->capacity) {                                                               \
      yoru_arraylist_resize((__arr_ptr), (__arr_ptr)->capacity * 2);                                                   \
    }                                                                                                                  \
    (__arr_ptr)->items[(__arr_ptr)->size++] = __value;                                                                 \
  } while (0);

#define yoru_arraylist_destroy(__arr_ptr)                                                                              \
  do {                                                                                                                 \
    Yoru_Guard_NotNull((__arr_ptr));                                                                                   \
    if (!(__arr_ptr)->items) {                                                                                         \
      yoru_allocator_dealloc((__arr_ptr)->allocator, (__arr_ptr)->items);                                              \
      (__arr_ptr)->items = NULL;                                                                                       \
    }                                                                                                                  \
    (__arr_ptr)->size      = 0;                                                                                        \
    (__arr_ptr)->capacity  = 0;                                                                                        \
    (__arr_ptr)->allocator = NULL;                                                                                     \
  } while (0)

#define yoru_arraylist_prepend(__arr_ptr, __value)                                                                     \
  do {                                                                                                                 \
    usize item_size = sizeof((__arr_ptr)->items[0]);                                                                   \
    Yoru_Guard_NotNull((__arr_ptr));                                                                                   \
    if ((__arr_ptr)->size + 1 > (__arr_ptr)->capacity) {                                                               \
      yoru_arraylist_resize((__arr_ptr), (__arr_ptr)->capacity * 2);                                                   \
      \                                                                                                                \
    }                                                                                                                  \
    memmove((__arr_ptr)->items + 1, (__arr_ptr)->items, (__arr_ptr)->size * item_size);                                \
    (__arr_ptr)->items[0] = __value;                                                                                   \
    ++(__arr_ptr)->size;                                                                                               \
  } while (0);

#define yoru_arraylist_resize(__arr_ptr, __new_capacity)                                                               \
  do {                                                                                                                 \
    usize item_size = sizeof((__arr_ptr)->items[0]);                                                                   \
    Yoru_Guard_NotNull((__arr_ptr));                                                                                   \
    Yoru_Opt maybe_new_ptr = yoru_allocator_realloc(                                                                   \
        (__arr_ptr)->allocator,                                                                                        \
        (__arr_ptr)->capacity * item_size,                                                                             \
        (__arr_ptr)->items,                                                                                            \
        (__new_capacity) * item_size);                                                                                 \
    assert(maybe_new_ptr.has_value && "could not append new value");                                                   \
    anyptr new_ptr        = maybe_new_ptr.ptr;                                                                         \
    (__arr_ptr)->items    = new_ptr;                                                                                   \
    (__arr_ptr)->size     = (__arr_ptr)->size < (__new_capacity) ? (__arr_ptr)->size : (__new_capacity);               \
    (__arr_ptr)->capacity = (__new_capacity);                                                                          \
  } while (0);

#endif // __YORU_DYNARRAY_H__
