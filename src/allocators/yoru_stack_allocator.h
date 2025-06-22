#ifndef __YORU_STACK_ALLOCATOR_H__
#define __YORU_STACK_ALLOCATOR_H__

#include "../yoru.h"

typedef struct
{

} StackAllocator_t;

StackAllocator_t *StackAllocator_Init(size_t capacity)
{
    TODO("Implement StackAllocator_Init");
}

void *StackAllocator_Alloc(StackAllocator_t *allocator, const size_t size)
{
    TODO("Implement StackAllocator_Alloc");
}

void StackAllocator_Reset(StackAllocator_t *allocator)
{
    TODO("Implement StackAllocator_Reset");
}

void StackAllocator_Free(StackAllocator_t *allocator)
{
    TODO("Implement StackAllocator_Free");
}

#endif