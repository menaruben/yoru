#ifndef __YORU_ARRAY_H__
#define __YORU_ARRAY_H__

#include <stddef.h>
#include <string.h>
#include "../allocators/yoru_allocators.h"
#include "../asserts/yoru_asserts.h"
#include "../inttypes/yoru_inttypes.h"

#define Array_t(T)       \
    struct               \
    {                    \
        T *items;        \
        size_t size;     \
        size_t capacity; \
    }

static inline void *array_alloc_items(Allocator_t *allocator, size_t item_size, size_t capacity);
static inline void array_free_items(Allocator_t *allocator, void *items);
static inline void *array_get_impl(void *items, size_t index, size_t size, size_t item_size);
static inline void array_set_impl(void *items, size_t *size, size_t capacity, size_t index, void *value, size_t item_size);
static inline void array_push_impl(void *items, size_t *size, size_t capacity, void *value, size_t item_size);

#define array_new(T, N, allocator_ptr) \
    {.items = (T *)array_alloc_items(allocator_ptr, sizeof(T), (N)), .size = 0, .capacity = (N)}

static inline void *array_alloc_items(Allocator_t *allocator, size_t item_size, size_t capacity)
{
    ASSERT_NOT_NULL(allocator);
    ASSERT(item_size > 0, "Item size must be greater than 0");
    ASSERT(capacity > 0, "Capacity must be greater than 0");
    void *items = allocator->alloc(allocator->context, item_size * capacity);
    ASSERT_NOT_NULL(items);
    return items;
}

static inline void array_free_items(Allocator_t *allocator, void *items)
{
    ASSERT_NOT_NULL(allocator);
    ASSERT_NOT_NULL(items);
    allocator->free(allocator->context, items);
}

static inline void *array_get_impl(void *items, size_t index, size_t size, size_t item_size)
{
    ASSERT(index >= 0 && index < size, "Index out of bounds");
}

#endif
