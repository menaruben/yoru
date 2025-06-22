#ifndef __YORU_ARRAY_H__
#define __YORU_ARRAY_H__

#include <stddef.h>
#include "../allocators/yoru_allocators.h"
#include "../math/yoru_math.h"
#include "../asserts/yoru_asserts.h"

#define Array_T(T)                                                            \
    typedef struct                                                            \
    {                                                                         \
        T *items;                                                             \
        size_t size;                                                          \
        size_t capacity;                                                      \
    } Array_##T;                                                              \
                                                                              \
    static inline Array_##T Array_##T##_Init(T *ptr, size_t capacity)         \
    {                                                                         \
        Array_##T arr;                                                        \
        ASSERT_NOT_EQUAL(ptr, NULL);                                          \
        arr.items = ptr;                                                      \
        arr.size = 0;                                                         \
        arr.capacity = capacity;                                              \
        return arr;                                                           \
    }                                                                         \
    static inline T *Array_##T##_Get(Array_##T *arr, size_t index)            \
    {                                                                         \
        index = usize_modulo(index, arr->capacity);                           \
        return &arr->items[index];                                            \
    }                                                                         \
    static inline void Array_##T##_Put(Array_##T *arr, size_t index, T value) \
    {                                                                         \
        index = usize_modulo(index, arr->capacity);                           \
        arr->items[index] = value;                                            \
    }

#endif