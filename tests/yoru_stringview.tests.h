#ifndef __YORU_STRINGVIEW_TESTS_H__
#define __YORU_STRINGVIEW_TESTS_H__

#include "../yoru.h"
#include "yoru_test_helpers.h"

/* ============================================================
   MODULE: StringView
   ============================================================ */

bool yoru_stringview_skip_test() {
  Yoru_Allocator     allocator = yoru_global_allocator_make();
  Yoru_StringBuilder sb        = {0};
  Yoru_StringView    sv        = {0};

  BUILD_SV_FROM_CSTR(&allocator, &sb, &sv, "abcdef");

  Yoru_StringView r = yoru_stringview_skip(&sv, 2);
  YORU_EXPECT_EQ_USIZE(4, r.length);
  YORU_EXPECT_EQ_MEM("cdef", r.data, 4);

  r = yoru_stringview_skip(&sv, 100);
  YORU_EXPECT_EQ_USIZE(0, r.length);

  yoru_stringbuilder_destroy(&sb);
  return true;

err:
  yoru_stringbuilder_destroy(&sb);
  return false;
}

bool yoru_stringview_skip_while_test() {
  Yoru_Allocator     allocator = yoru_global_allocator_make();
  Yoru_StringBuilder sb        = {0};
  Yoru_StringView    sv        = {0};

  BUILD_SV_FROM_CSTR(&allocator, &sb, &sv, "   hello");

  Yoru_StringView r = yoru_stringview_skip_while(&sv, is_space);
  YORU_EXPECT_EQ_USIZE(5, r.length);
  YORU_EXPECT_EQ_MEM("hello", r.data, 5);

  yoru_stringbuilder_destroy(&sb);
  return true;

err:
  yoru_stringbuilder_destroy(&sb);
  return false;
}

bool yoru_stringview_take_test() {
  Yoru_Allocator     allocator = yoru_global_allocator_make();
  Yoru_StringBuilder sb        = {0};
  Yoru_StringView    sv        = {0};

  BUILD_SV_FROM_CSTR(&allocator, &sb, &sv, "abcdef");

  Yoru_StringView r = yoru_stringview_take(&sv, 3);
  YORU_EXPECT_EQ_USIZE(3, r.length);
  YORU_EXPECT_EQ_MEM("abc", r.data, 3);

  r = yoru_stringview_take(&sv, 100);
  YORU_EXPECT_EQ_USIZE(6, r.length);

  yoru_stringbuilder_destroy(&sb);
  return true;

err:
  yoru_stringbuilder_destroy(&sb);
  return false;
}

bool yoru_stringview_take_while_test() {
  Yoru_Allocator     allocator = yoru_global_allocator_make();
  Yoru_StringBuilder sb        = {0};
  Yoru_StringView    sv        = {0};

  BUILD_SV_FROM_CSTR(&allocator, &sb, &sv, "abc123");

  Yoru_StringView r = yoru_stringview_take_while(&sv, is_alpha);
  YORU_EXPECT_EQ_USIZE(3, r.length);
  YORU_EXPECT_EQ_MEM("abc", r.data, 3);

  yoru_stringbuilder_destroy(&sb);
  return true;

err:
  yoru_stringbuilder_destroy(&sb);
  return false;
}

bool yoru_stringview_has_prefix_suffix_infix_test() {
  Yoru_Allocator     allocator = yoru_global_allocator_make();
  Yoru_StringBuilder sb        = {0};
  Yoru_StringView    sv        = {0};

  BUILD_SV_FROM_CSTR(&allocator, &sb, &sv, "hello world");

  YORU_EXPECT_TRUE(yoru_stringview_has_prefix(&sv, "hello", 5));
  YORU_EXPECT_TRUE(yoru_stringview_has_suffix(&sv, "world", 5));
  YORU_EXPECT_TRUE(yoru_stringview_has_infix(&sv, "lo wo", 5));
  YORU_EXPECT_TRUE(!yoru_stringview_has_infix(&sv, "WORLD", 5));

  yoru_stringbuilder_destroy(&sb);
  return true;

err:
  yoru_stringbuilder_destroy(&sb);
  return false;
}

bool yoru_stringview_is_empty_test() {
  Yoru_Allocator     allocator = yoru_global_allocator_make();
  Yoru_StringBuilder sb        = {0};
  Yoru_StringView    sv        = {0};

  BUILD_SV_FROM_CSTR(&allocator, &sb, &sv, "");

  YORU_EXPECT_TRUE(yoru_stringview_is_empty(&sv));

  yoru_stringbuilder_destroy(&sb);
  return true;

err:
  yoru_stringbuilder_destroy(&sb);
  return false;
}

bool yoru_stringview_trim_test() {
  Yoru_Allocator     allocator = yoru_global_allocator_make();
  Yoru_StringBuilder sb        = {0};
  Yoru_StringView    sv        = {0};

  BUILD_SV_FROM_CSTR(&allocator, &sb, &sv, "   hello   ");

  Yoru_StringView r = yoru_stringview_trim(&sv, YORU_TRIM_LEFT | YORU_TRIM_RIGHT);

  YORU_EXPECT_EQ_USIZE(5, r.length);
  YORU_EXPECT_EQ_MEM("hello", r.data, 5);

  yoru_stringbuilder_destroy(&sb);
  return true;

err:
  yoru_stringbuilder_destroy(&sb);
  return false;
}

bool yoru_stringview_trim_while_test() {
  Yoru_Allocator     allocator = yoru_global_allocator_make();
  Yoru_StringBuilder sb        = {0};
  Yoru_StringView    sv        = {0};

  BUILD_SV_FROM_CSTR(&allocator, &sb, &sv, "xxxhelloxxx");

  Yoru_StringView r = yoru_stringview_trim_while(&sv, YORU_TRIM_LEFT | YORU_TRIM_RIGHT, is_x);

  YORU_EXPECT_EQ_USIZE(5, r.length);
  YORU_EXPECT_EQ_MEM("hello", r.data, 5);

  yoru_stringbuilder_destroy(&sb);
  return true;

err:
  yoru_stringbuilder_destroy(&sb);
  return false;
}

bool yoru_stringview_split_by_char_test() {
  Yoru_Allocator     allocator = yoru_global_allocator_make();
  Yoru_StringBuilder sb        = {0};
  Yoru_StringView    sv        = {0};

  BUILD_SV_FROM_CSTR(&allocator, &sb, &sv, "Hello World! This is a test  :) ");

  Yoru_StringViews fields = yoru_stringview_split_by_char(&sv, &allocator, USIZE_MAX, ' ', true);

  YORU_EXPECT_EQ_USIZE(7, fields.size);

  yoru_stringbuilder_destroy(&sb);
  return true;

err:
  yoru_stringbuilder_destroy(&sb);
  return false;
}

#endif
