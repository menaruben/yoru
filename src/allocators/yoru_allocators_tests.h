#include "../testing/yoru_tests.h"

// Arena Allocator tests
#include "../testing/yoru_tests.h"
#include "yoru_arena_allocator.h"

YoruTestResult_t test_arena_allocator(void)
{
    size_t arena_size = 128;
    ArenaAllocator_t *arena = ArenaAllocator_Init(arena_size);
    if (!arena)
        return (YoruTestResult_t){false, "ArenaAllocator_Init returned NULL"};

    void *ptr1 = ArenaAllocator_Alloc(arena, 32);
    if (!ptr1)
    {
        ArenaAllocator_Free(arena);
        return (YoruTestResult_t){false, "ArenaAllocator_Alloc failed for 32 bytes"};
    }

    void *ptr2 = ArenaAllocator_Alloc(arena, 96);
    if (!ptr2)
    {
        ArenaAllocator_Free(arena);
        return (YoruTestResult_t){false, "ArenaAllocator_Alloc failed for 96 bytes"};
    }

    void *ptr3 = ArenaAllocator_Alloc(arena, 1);
    if (ptr3)
    {
        ArenaAllocator_Free(arena);
        return (YoruTestResult_t){false, "ArenaAllocator_Alloc should fail when out of space"};
    }

    ArenaAllocator_Free(arena);
    return (YoruTestResult_t){true, "test_arena_allocator passed"};
}

// Heap Allocator tests
#include "yoru_heap_allocator.h"

YoruTestResult_t test_heap_allocator(void)
{
    void *ptr = HeapAllocator_Alloc(64);
    if (!ptr)
        return (YoruTestResult_t){false, "HeapAllocator_Alloc failed"};

    void *ptr2 = HeapAllocator_Realloc(ptr, 128);
    if (!ptr2)
    {
        HeapAllocator_Free(ptr);
        return (YoruTestResult_t){false, "HeapAllocator_Realloc failed"};
    }

    HeapAllocator_Free(ptr2);
    return (YoruTestResult_t){true, "test_heap_allocator passed"};
}