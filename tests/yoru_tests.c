#define YORU_IMPL
#include "../yoru.h"
#include "yoru_stringview.tests.h"

#include <stdbool.h>
#include <stdio.h>

typedef bool (*Yoru_TestFn)(void);

typedef struct {
  const char *name;
  Yoru_TestFn fn;
} Yoru_Test;

#define COLOR_GREEN "\x1b[32m"
#define COLOR_RED "\x1b[31m"
#define COLOR_RESET "\x1b[0m"

static bool test_run(const Yoru_Test *test) {
  bool ok = test->fn();
  if (ok) {
    printf(COLOR_GREEN "OK" COLOR_RESET "   %s\n", test->name);
  } else {
    printf(COLOR_RED "FAIL" COLOR_RESET " %s\n", test->name);
  }
  return ok;
}

int main(void) {
  Yoru_Test tests[] = {
      {"stringview_skip", yoru_stringview_skip_test},
      {"stringview_skip_while", yoru_stringview_skip_while_test},
      {"stringview_take", yoru_stringview_take_test},
      {"stringview_take_while", yoru_stringview_take_while_test},
      {"stringview_has_prefix_suffix_infix", yoru_stringview_has_prefix_suffix_infix_test},
      {"stringview_is_empty", yoru_stringview_is_empty_test},
      {"stringview_trim", yoru_stringview_trim_test},
      {"stringview_trim_while", yoru_stringview_trim_while_test},
      {"stringview_split_by_char", yoru_stringview_split_by_char_test},
  };

  usize test_count = sizeof(tests) / sizeof(tests[0]);
  usize passed     = 0;

  printf("Running %zu tests:\n\n", test_count);

  for (usize i = 0; i < test_count; ++i) {
    if (test_run(&tests[i])) { passed++; }
  }

  printf("\nSummary: %zu / %zu tests passed\n", passed, test_count);
  return passed == test_count ? 0 : 1;
}
