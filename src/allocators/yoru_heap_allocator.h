#ifndef __YORU_HEAP_ALLOCATOR_H__
#define __YORU_HEAP_ALLOCATOR_H__

#include <stddef.h>
#include <stdlib.h>

void *HeapAllocator_Alloc(size_t size)
{
    return malloc(size);
}

void *HeapAllocator_Realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

void HeapAllocator_Free(void *ptr)
{
    free(ptr);
}

#endif