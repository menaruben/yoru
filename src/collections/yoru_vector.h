#ifndef __YORU_VECTOR_H__
#define __YORU_VECTOR_H__

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../asserts/yoru_asserts.h"
#include "../inttypes/yoru_inttypes.h"
#include "../allocators/yoru_allocators.h"

#define INITIAL_CAPACITY 16

static inline void vec_append_impl(void **items, void *item, size_t item_size, size_t *size, size_t *capacity, Allocator_t *allocator);

static inline void *vec_get_impl(void *items, size_t index, size_t size, size_t item_size);

static inline void *vec_alloc_items(Allocator_t *allocator, size_t item_size);

static inline void vec_destroy_items_impl(void *items, Allocator_t *allocator);

#define Vec_t(T)         \
    struct               \
    {                    \
        T *items;        \
        size_t size;     \
        size_t capacity; \
    }

#define vec_new(T, allocator_ptr) \
    {.items = vec_alloc_items(allocator_ptr, sizeof(T)), .size = 0, .capacity = INITIAL_CAPACITY}

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

#define vec_get(vec, index) \
    (typeof(vec.items))vec_get_impl((void *)vec.items, index, vec.size, sizeof(*(vec).items))

static inline void *vec_alloc_items(Allocator_t *allocator, size_t item_size)
{
    ASSERT_NOT_NULL(allocator);
    ASSERT_NOT_NULL(allocator->alloc);
    ASSERT_NOT_NULL(allocator->free);
    // context can be null because heap allocator does NOT have a context
    void *items = allocator->alloc(allocator->context, INITIAL_CAPACITY * item_size);
    ASSERT_NOT_NULL(items);
    return items;
}

static inline void vec_destroy_items_impl(void *items, Allocator_t *allocator)
{
    ASSERT_NOT_NULL(allocator);
    ASSERT_NOT_NULL(allocator->free);
    if (items != NULL)
    {
        allocator->free(allocator->context, items);
    }
}

static inline void vec_append_impl(
    void **items,
    void *item,
    size_t item_size,
    size_t *size,
    size_t *capacity,
    Allocator_t *allocator)
{
    if (*size >= *capacity)
    {
        while (*capacity <= *size)
        {
            *capacity *= 2; // Double the capacity until it is sufficient
        }
        void *new_items = allocator->realloc(*items, (*capacity) * item_size);
        ASSERT_NOT_NULL(new_items);
        *items = new_items;
    }
    size_t index = *size * item_size;
    memcpy((char *)(*items) + index, item, item_size); // Copy the item into the vector
    (*size)++;
}

static inline void *vec_get_impl(void *items, size_t index, size_t size, size_t item_size)
{
    bool within_bounds = index < size && index >= 0;
    ASSERT(within_bounds, "Index out of bounds.");

    void *item_ptr = (char *)items + index * item_size;
    ASSERT(item_ptr != NULL, "Item pointer is NULL.");

    return item_ptr;
}

#endif