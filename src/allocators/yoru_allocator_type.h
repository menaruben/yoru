#ifndef __YORU_ALLOCATOR_TYPE_H__
#define __YORU_ALLOCATOR_TYPE_H__

#include <stdlib.h>
#include <stddef.h>
#include "../functions/yoru_functions.h"

/**
 * @brief Structure representing a memory allocator.
 *
 * This struct defines the interface and data required for a custom memory allocator.
 * It typically contains function pointers for allocation, deallocation, and possibly
 * other memory management operations, as well as any state or configuration data
 * needed by the allocator.
 * * The `context` field can be used to store any additional data needed by the allocator,
 *   such as a pointer to an arena or other allocator-specific state.
 */
typedef struct Allocator_t
{
  FUNC(void *, alloc, void *, size_t);
  FUNC(void, free, void *, void *);
  FUNC(void *, realloc, void *, size_t);
  void *context;
} Allocator_t;

#endif
