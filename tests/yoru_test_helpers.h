#ifndef __YORU_TEST_HELPERS_H__
#define __YORU_TEST_HELPERS_H__

#include "../yoru.h"
#include <stdio.h>

/* ============================================================
   Assertion macros (value-based)
   ============================================================ */

#define YORU_EXPECT_TRUE(actual)                                                                                       \
  do {                                                                                                                 \
    if (!(actual)) {                                                                                                   \
      fprintf(stderr, "%s:%d at %s:\n  expected: true\n  actual:   false\n", __FILE__, __LINE__, __func__);            \
      goto err;                                                                                                        \
    }                                                                                                                  \
  } while (0)

#define YORU_EXPECT_EQ_USIZE(expected, actual)                                                                         \
  do {                                                                                                                 \
    usize _exp = (expected);                                                                                           \
    usize _act = (actual);                                                                                             \
    if (_exp != _act) {                                                                                                \
      fprintf(stderr, "%s:%d at %s:\n  expected: %zu\n  actual:   %zu\n", __FILE__, __LINE__, __func__, _exp, _act);   \
      goto err;                                                                                                        \
    }                                                                                                                  \
  } while (0)

#define YORU_EXPECT_EQ_MEM(expected_ptr, actual_ptr, size)                                                             \
  do {                                                                                                                 \
    if (memcmp((expected_ptr), (actual_ptr), (size)) != 0) {                                                           \
      fprintf(stderr, "%s:%d at %s:\n  memory mismatch (size=%zu)\n", __FILE__, __LINE__, __func__, (usize)(size));    \
      goto err;                                                                                                        \
    }                                                                                                                  \
  } while (0)

/* ============================================================
   Helpers
   ============================================================ */

#define BUILD_SV_FROM_CSTR(allocator, builder, view, cstr)                                                             \
  do {                                                                                                                 \
    yoru_stringbuilder_init((allocator), (builder));                                                                   \
    YORU_EXPECT_TRUE(yoru_stringbuilder_append_cstr((builder), (cstr), strlen(cstr)));                                 \
    YORU_EXPECT_TRUE(yoru_stringbuilder_to_stringview((builder), (view)));                                             \
  } while (0)

static bool is_space(u8 c) {
  return c == ' ';
}

static bool is_alpha(u8 c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool is_x(u8 c) {
  return c == 'x';
}

#endif
