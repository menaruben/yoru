#ifndef __YORU_ALLOCATOR_TYPE_H__
#define __YORU_ALLOCATOR_TYPE_H__

#include <stdlib.h>
#include <stddef.h>
#include "../functions/yoru_functions.h"
#include "../results/yoru_results.h"

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
typedef struct Yoru_Allocator_t
{
  FUNC(void *, alloc, void *, size_t);
  FUNC(void, free, void *, void *);
  FUNC(void *, realloc, void *, size_t);

  // Optional functions for error handling
  Yoru_Result_t (*alloc_try)(void *, size_t);
  Yoru_Result_t (*realloc_try)(void *, void *, size_t);
  Yoru_Result_t (*free_try)(void *, void *);
  void *context;
} Yoru_Allocator_t;

#endif
