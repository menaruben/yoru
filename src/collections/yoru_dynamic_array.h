#ifndef __YORU_DYNAMIC_ARRAY_H__
#define __YORU_DYNAMIC_ARRAY_H__

#include "../asserts/yoru_asserts.h"
#include "../allocators/yoru_allocators.h"
#include "../math/yoru_math.h"
#include <stddef.h>

#define INITIAL_CAPACITY 16

/*
 * It is import to note that the dynamic array will ALYWAYS use the heap allocator
 * Other allocators are just not suitable for dynarmic arrays.
 */

#define DynArray_T(type)                                                                     \
    typedef struct                                                                           \
    {                                                                                        \
        type *items;                                                                         \
        size_t size;                                                                         \
        size_t capacity;                                                                     \
    } DynArray_##type;                                                                       \
                                                                                             \
    static inline DynArray_##type DynArray_##type##_Init()                                   \
    {                                                                                        \
        DynArray_##type arr;                                                                 \
        void *ptr = HeapAllocator_Alloc(                                                     \
            sizeof(type) * INITIAL_CAPACITY);                                                \
        ASSERT_NOT_EQUAL(ptr, NULL);                                                         \
        arr.items = (type *)ptr;                                                             \
        arr.size = 0;                                                                        \
        arr.capacity = INITIAL_CAPACITY;                                                     \
        return arr;                                                                          \
    }                                                                                        \
    static inline void DynArray_##type##_Free(DynArray_##type *arr)                          \
    {                                                                                        \
        ASSERT_NOT_EQUAL(arr->items, NULL);                                                  \
        HeapAllocator_Free(arr->items);                                                      \
        arr->items = NULL;                                                                   \
        arr->size = 0;                                                                       \
        arr->capacity = 0;                                                                   \
    }                                                                                        \
    static inline type *DynArray_##type##_Get(                                               \
        DynArray_##type *arr,                                                                \
        size_t index)                                                                        \
    {                                                                                        \
        ASSERT_NOT_EQUAL(arr->items, NULL);                                                  \
        index = usize_modulo(index, arr->capacity);                                          \
        return &arr->items[index];                                                           \
    }                                                                                        \
    static inline void DynArray_##type##_Put(DynArray_##type *arr, size_t index, type value) \
    {                                                                                        \
        ASSERT_NOT_EQUAL(arr->items, NULL);                                                  \
        if (index >= arr->capacity)                                                          \
        {                                                                                    \
            size_t new_capacity = arr->capacity * 2;                                         \
            type *new_items = (type *)HeapAllocator_Realloc(                                 \
                arr->items, sizeof(type) * new_capacity);                                    \
            ASSERT_NOT_EQUAL(new_items, NULL);                                               \
            arr->items = new_items;                                                          \
            arr->capacity = new_capacity;                                                    \
        }                                                                                    \
        arr->items[index] = value;                                                           \
        arr->size++;                                                                         \
    }

#endif