#include "../testing/yoru_tests.h"

// Arena Allocator tests
#include "yoru_tests.h"
#include "../allocators/yoru_arena_allocator.h"
#include "../allocators/yoru_heap_allocator.h"

Yoru_TestResult_t test_arena_allocator(void)
{
    size_t arena_size = 128;
    Yoru_Allocator_t *allocator = Yoru_ArenaAllocator_new(arena_size);
    if (!allocator)
        return (Yoru_TestResult_t){false, "Yoru_ArenaAllocator_Init returned NULL"};

    void *ptr1 = allocator->alloc(allocator->context, 32);
    if (!ptr1)
    {
        allocator->free(allocator->context, NULL);
        return (Yoru_TestResult_t){false, "ArenaAllocator_Alloc failed for 32 bytes"};
    }

    void *ptr2 = allocator->realloc(allocator->context, 96);
    if (ptr2)
    {
        allocator->free(allocator->context, NULL);
        return (Yoru_TestResult_t){false, "ArenaAllocator_Realloc is not supported on ArenaAllocator"};
    }

    void *ptr3 = allocator->alloc(allocator->context, arena_size);
    if (ptr3)
    {
        return (Yoru_TestResult_t){false, "ArenaAllocator_Alloc should fail when out of space"};
    }

    allocator->free(allocator->context, NULL);
    return (Yoru_TestResult_t){true, "test_arena_allocator passed"};
}

// Heap Allocator tests

Yoru_TestResult_t test_heap_allocator(void)
{
    Yoru_Allocator_t *allocator = Yoru_HeapAllocator_new();
    if (!allocator)
        return (Yoru_TestResult_t){false, "Yoru_HeapAllocator_new returned NULL"};

    void *ptr = allocator->alloc(allocator->context, 64);
    if (!ptr)
        return (Yoru_TestResult_t){false, "HeapAllocator_Alloc failed"};

    void *ptr2 = allocator->realloc(allocator->context, 128);
    if (!ptr2)
    {
        allocator->free(allocator->context, ptr);
        return (Yoru_TestResult_t){false, "HeapAllocator_Realloc failed"};
    }

    allocator->free(allocator->context, ptr2);
    return (Yoru_TestResult_t){true, "test_heap_allocator passed"};
}