#define YORU_IMPL
#include "../yoru.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

void print_bytes(const u8 *ptr, usize n);

int main() {
  Yoru_GlobalAllocator allocator = yoru_global_allocator_make();
  cstr                 filepath  = "./files.c";
  usize                file_size = yoru_file_get_size(filepath);
  printf("file is %zu bytes long\n", file_size);

  usize       offset                  = 10;
  usize       max_bytes               = 13;
  Yoru_String content                 = yoru_file_read(&allocator, filepath);
  Yoru_String content_skip_10_take_13 = yoru_file_read_exact(&allocator, filepath, offset, max_bytes);
  assert(content.data);
  assert(content_skip_10_take_13.data);

  print_bytes(content.data + offset, max_bytes);
  print_bytes(content_skip_10_take_13.data, max_bytes);

  assert(memcmp(content.data + offset, content_skip_10_take_13.data, max_bytes) == 0);
  printf("content:\n" Yoru_String_Fmt "", Yoru_String_Fmt_Args(&content));
  return 0;
}

void print_bytes(const u8 *ptr, usize n) {
  for (usize i = 0; i < n; ++i)
    printf("%02x", ptr[i]);

  printf("\n");
}
