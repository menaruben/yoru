#define YORU_IMPL
#include "../src/yoru.h"

#include <stdio.h>

int main() {
  Yoru_GlobalAllocator allocator = yoru_global_allocator_make();
  Yoru_String          str       = {0};

  // -1 otherwise includes null terminator
  if (!yoru_string_from_str(&allocator, "-yoru string", sizeof("-yoru string") - 1, &str)) { return 1; }

  Yoru_StringBuilder sb = {0};
  yoru_stringbuilder_init(&allocator, &sb);

  yoru_stringbuilder_append_char(&sb, 'c');
  yoru_stringbuilder_append_cstr(&sb, "-cstr", sizeof("-cstr") - 1);
  yoru_stringbuilder_append_string(&sb, &str);

  Yoru_String res = {0};
  if (!yoru_stringbuilder_to_string(&sb, &res)) { return 1; }

  printf("res = `%.*s`", (int)res.length, res.data);

  yoru_stringbuilder_destroy(&sb);
  return 0;
}
