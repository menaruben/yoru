#ifndef __YORU_ARRAY_H__
#define __YORU_ARRAY_H__

#include <stddef.h>
#include "../allocators/yoru_allocators.h"
#include "../math/yoru_math.h"
#include "../asserts/yoru_asserts.h"

#define Array_T(type)                                                                        \
    typedef struct                                                                           \
    {                                                                                        \
        type *items;                                                                         \
        size_t size;                                                                         \
        size_t capacity;                                                                     \
    } Array_##type;                                                                          \
                                                                                             \
    static inline Array_##type Array_##type##_Init(ArenaAllocator_t *arena, size_t capacity) \
    {                                                                                        \
        Array_##type arr;                                                                    \
        void *ptr = ArenaAllocator_Allocate(arena, sizeof(type) * capacity);                 \
        ASSERT_NOT_EQUAL(ptr, NULL);                                                         \
        arr.items = (type *)ArenaAllocator_Allocate(arena, sizeof(type) * capacity);         \
        arr.size = 0;                                                                        \
        arr.capacity = capacity;                                                             \
        return arr;                                                                          \
    }                                                                                        \
                                                                                             \
    static inline type *Array_##type##_Get(Array_##type *arr, size_t index)                  \
    {                                                                                        \
        index = usize_modulo(index, arr->capacity);                                          \
        return &arr->items[index];                                                           \
    }                                                                                        \
    static inline void Array_##type##_Put(Array_##type *arr, size_t index, type value)       \
    {                                                                                        \
        index = usize_modulo(index, arr->capacity);                                          \
        arr->items[index] = value;                                                           \
    }

#endif