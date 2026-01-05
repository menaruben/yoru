#ifndef __YORU_STRING_H__
#define __YORU_STRING_H__

#include "../allocators/allocator.h"
#include "../types/types.h"

/// @brief Immutable and sized NON-NULLTERMINATED c-string
typedef struct Yoru_String {
  u8             *data;
  usize           length;
  Yoru_Allocator *allocator;
} Yoru_String;

bool yoru_string_make(Yoru_Allocator *allocator, usize length, Yoru_String *out_string);
bool yoru_string_from_str(Yoru_Allocator *allocator, const char *s, usize length, Yoru_String *out_string);
bool yoru_string_copy(Yoru_Allocator *allocator, Yoru_String *src, Yoru_String *dest);
bool yoru_string_substring(Yoru_String *s, usize start, usize end, Yoru_String *out_string);
void yoru_string_destroy(Yoru_String *s);

#ifdef YORU_IMPL
  #include "../contracts/contracts.h"
  #include <string.h>
  #include <assert.h>

bool yoru_string_make(Yoru_Allocator *allocator, usize length, Yoru_String *out_string) {
  Yoru_Guard_NotNull(allocator);
  Yoru_Guard_NotNull(out_string);

  Yoru_Opt maybe_data = yoru_allocator_alloc(allocator, length * sizeof(u8));
  if (!maybe_data.has_value) return false;

  out_string->data      = maybe_data.ptr;
  out_string->length    = length;
  out_string->allocator = allocator;
  return true;
}

bool yoru_string_from_str(Yoru_Allocator *allocator, const char *s, usize length, Yoru_String *out_string) {
  if (!yoru_string_make(allocator, length, out_string)) return false;
  for (usize i = 0; i < length; ++i)
    out_string->data[i] = (u8)s[i];

  return true;
}

bool yoru_string_copy(Yoru_Allocator *allocator, Yoru_String *src, Yoru_String *dest) {
  Yoru_Guard_NotNull(allocator);
  Yoru_Guard_NotNull(src);
  Yoru_Guard_NotNull(dest);

  if (!yoru_string_make(allocator, src->length, dest)) return false;
  memcpy(dest->data, src->data, src->length);
  return true;
}

bool yoru_string_substring(Yoru_String *s, usize start, usize end, Yoru_String *out_string) {
  Yoru_Guard_NotNull(out_string);
  Yoru_Guard_NotNull(s);
  Yoru_Guard_NotNull(s->data);
  Yoru_Guard_NotNull(s->allocator);
  assert(start < s->length && "index must be within length");
  assert(end < s->length && "index must be within length");
  assert(start < end && "start index must be less than end");

  usize length          = end - start;
  out_string->data      = s->data + start;
  out_string->length    = length;
  out_string->allocator = s->allocator;
  return true;
}

void yoru_string_destroy(Yoru_String *s) {
  Yoru_Guard_NotNull(s);
  Yoru_Guard_NotNull(s->data);
  Yoru_Guard_NotNull(s->allocator);

  yoru_allocator_dealloc(s->allocator, s->data);
  s->data      = NULL;
  s->length    = 0;
  s->allocator = NULL;
}

#endif // YORU_IMPL
#endif // __YORU_STRING_H__
