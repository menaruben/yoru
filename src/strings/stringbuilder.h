#ifndef __YORU_STRINGBUILDER_H__
#define __YORU_STRINGBUILDER_H__

#include "../types/types.h"
#include "../collections/arraylist.h"
#include "../allocators/allocator.h"
#include "stringview.h"

typedef Yoru_ArrayList_T(u8) Yoru_StringBuilder;

void yoru_stringbuilder_init(Yoru_Allocator *allocator, Yoru_StringBuilder *sb);
void yoru_stringbuilder_destroy(Yoru_StringBuilder *sb);
bool yoru_stringbuilder_append_char(Yoru_StringBuilder *sb, char c);
bool yoru_stringbuilder_append_cstr(Yoru_StringBuilder *sb, const char *, usize length);
bool yoru_stringbuilder_append_string(Yoru_StringBuilder *sb, const Yoru_String *s);
bool yoru_stringbuilder_to_string(Yoru_StringBuilder *sb, Yoru_String *out_string);

#ifdef YORU_IMPL
  #include "../contracts/contracts.h"
  #include <stdio.h>
  #include <stdarg.h>
  #include <string.h>

void yoru_stringbuilder_init(Yoru_Allocator *allocator, Yoru_StringBuilder *sb) {
  Yoru_Guard_NotNull(sb);
  Yoru_Guard_NotNull(allocator);
  yoru_arraylist_init(sb, allocator, 0); // uses default initial capacity
}

void yoru_stringbuilder_destroy(Yoru_StringBuilder *sb) {
  Yoru_Guard_NotNull(sb);
  yoru_arraylist_destroy(sb);
}

bool yoru_stringbuilder_to_string(Yoru_StringBuilder *sb, Yoru_String *out_string) {
  Yoru_Guard_NotNull(sb);
  Yoru_Guard_NotNull(sb->items);
  Yoru_Guard_NotNull(sb->allocator);
  Yoru_Guard_NotNull(out_string);
  Yoru_String temp_res = {0};
  if (!yoru_string_from_str(sb->allocator, (const char *)sb->items, sb->size, &temp_res)) return false;
  if (!yoru_string_copy(sb->allocator, &temp_res, out_string)) return false;
  return true;
}

bool yoru_stringbuilder_append_char(Yoru_StringBuilder *sb, char c) {
  Yoru_Guard_NotNull(sb);
  Yoru_Guard_NotNull(sb->items);
  Yoru_Guard_NotNull(sb->allocator);
  yoru_arraylist_append(sb, (u8)c);
  return true;
}

bool yoru_stringbuilder_append_cstr(Yoru_StringBuilder *sb, const char *cstr, usize length) {
  Yoru_Guard_NotNull(sb);
  Yoru_Guard_NotNull(sb->items);
  Yoru_Guard_NotNull(sb->allocator);
  Yoru_Guard_NotNull(cstr);

  for (usize i = 0; i < length; ++i)
    if (!yoru_stringbuilder_append_char(sb, (u8)cstr[i])) return false;

  return true;
}

bool yoru_stringbuilder_append_string(Yoru_StringBuilder *sb, const Yoru_String *s) {
  Yoru_Guard_NotNull(sb);
  Yoru_Guard_NotNull(sb->items);
  Yoru_Guard_NotNull(sb->allocator);
  Yoru_Guard_NotNull(s);

  for (usize i = 0; i < s->length; ++i)
    if (!yoru_stringbuilder_append_char(sb, s->data[i])) return false;

  return true;
}

#endif
#endif
