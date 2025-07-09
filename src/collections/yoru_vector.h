#ifndef __YORU_VECTOR_H__
#define __YORU_VECTOR_H__

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../asserts/yoru_asserts.h"
#include "../inttypes/yoru_inttypes.h"
#include "../allocators/yoru_allocators.h"
#include "../yoru_defs.h"

#define INITIAL_CAPACITY 16

YORU_HELPER void vec_append_impl(void **items, void *item, size_t item_size, size_t *size, size_t *capacity, Yoru_Allocator_t *allocator);
YORU_HELPER void *vec_get_impl(void *items, size_t index, size_t capacity, size_t item_size);
YORU_HELPER void vec_set_impl(void *items, size_t index, void *item, size_t item_size, size_t capacity);
YORU_HELPER bool is_within_bounds(size_t index, size_t capacity);
YORU_HELPER void *vec_alloc_items(Yoru_Allocator_t *allocator, size_t item_size);
YORU_HELPER void vec_destroy_items_impl(void *items, Yoru_Allocator_t *allocator);
YORU_HELPER void *vec_grow_items(Yoru_Allocator_t *allocator, void **items, size_t *size, size_t *capacity, size_t item_size);

#define Vec_t(T)         \
    struct               \
    {                    \
        T *items;        \
        size_t size;     \
        size_t capacity; \
    }

#define vec_new(T, allocator_ptr) \
    {.items = (T *)vec_alloc_items(allocator_ptr, sizeof(T)), .size = 0, .capacity = INITIAL_CAPACITY}

#define vec_destroy(vec, allocator_ptr)                             \
    do                                                              \
    {                                                               \
        vec_destroy_items_impl((void *)(vec).items, allocator_ptr); \
        (vec).items = NULL;                                         \
        (vec).size = 0;                                             \
        (vec).capacity = 0;                                         \
    } while (0)

#define vec_append(vec, item, allocator_ptr) \
    vec_append_impl((void **)&((vec).items), &(item), sizeof(*(vec).items), &(vec).size, &(vec).capacity, allocator_ptr)

#define vec_get(T_ptr, vec, index) \
    (T_ptr) vec_get_impl((void *)vec.items, index, vec.size, sizeof(*(vec).items))

#define vec_set(vec, index, item) \
    vec_set_impl((void *)vec.items, index, &(item), sizeof(*(vec).items), vec.size)

YORU_HELPER void *vec_alloc_items(Yoru_Allocator_t *allocator, size_t item_size)
{
    YORU_ASSERT_NOT_NULL(allocator);
    YORU_ASSERT_NOT_NULL(allocator->alloc);
    YORU_ASSERT_NOT_NULL(allocator->free);
    // context can be null because heap allocator does NOT have a context
    void *items = allocator->alloc(allocator->context, INITIAL_CAPACITY * item_size);
    YORU_ASSERT_NOT_NULL(items);
    return items;
}

YORU_HELPER void vec_destroy_items_impl(void *items, Yoru_Allocator_t *allocator)
{
    YORU_ASSERT_NOT_NULL(allocator);
    YORU_ASSERT_NOT_NULL(allocator->free);
    if (items != NULL)
    {
        allocator->free(allocator->context, items);
    }
}

YORU_HELPER void vec_append_impl(
    void **items,
    void *item,
    size_t item_size,
    size_t *size,
    size_t *capacity,
    Yoru_Allocator_t *allocator)
{
    if (*size >= *capacity)
    {
        *items = vec_grow_items(allocator, items, size, capacity, item_size);
    }
    size_t index = *size * item_size;
    memcpy((char *)(*items) + index, item, item_size);
    (*size)++;
}

YORU_HELPER void vec_set_impl(void *items, size_t index, void *item, size_t item_size, size_t capacity)
{
    YORU_ASSERT(is_within_bounds(index, capacity), "Index out of bounds.");
    void *item_ptr = (char *)items + index * item_size;
    YORU_ASSERT_NOT_NULL(item_ptr);
    memcpy(item_ptr, item, item_size);
}

YORU_HELPER void *vec_get_impl(void *items, size_t index, size_t capacity, size_t item_size)
{
    YORU_ASSERT(is_within_bounds(index, capacity), "Index out of bounds.");
    void *item_ptr = (char *)items + index * item_size;
    YORU_ASSERT_NOT_NULL(item_ptr);
    return item_ptr;
}

YORU_HELPER bool is_within_bounds(size_t index, size_t capacity)
{
    return index < capacity && index >= 0;
}

YORU_HELPER void *vec_grow_items(Yoru_Allocator_t *allocator, void **items, size_t *size, size_t *capacity, size_t item_size)
{
    YORU_ASSERT_NOT_NULL(allocator);
    YORU_ASSERT_NOT_NULL(allocator->realloc);
    YORU_ASSERT_NOT_NULL(items);
    YORU_ASSERT_NOT_NULL(*items);
    while (*capacity <= *size)
    {
        *capacity *= 2;
    }
    void *new_items = allocator->realloc(*items, (*capacity) * item_size);
    YORU_ASSERT_NOT_NULL(new_items);
    *items = new_items;
    return new_items;
}

#endif