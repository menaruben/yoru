#ifndef __YORU_STRING_H__
#define __YORU_STRING_H__

#include "../allocators/allocator.h"
#include "../types/types.h"

/// @brief Immutable and sized nullterminated c-string
typedef struct Yoru_String {
  char           *data;
  usize           length;
  Yoru_Allocator *allocator;
} Yoru_String;

Yoru_String yoru_string_make(Yoru_Allocator *allocator);

#ifdef YORU_IMPL
  #include "../contracts/contracts.h"

Yoru_String yoru_string_make(Yoru_Allocator *allocator) {
  Yoru_Guard_NotNull(allocator);
}

#endif
#endif
