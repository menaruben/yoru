#define YORU_IMPL
#include "../src/yoru.h"

#include <assert.h>
#include <stdio.h>

typedef struct {
  Yoru_Implement_DynArray(int);
} IntDynArray;

void print_dynarray(IntDynArray arr);

int main() {
  Yoru_GlobalAllocator allocator = yoru_global_allocator_make();

  IntDynArray arr = {0};
  if (!yoru_dynarray_init(&arr, &allocator)) { return 1; }

  for (usize i = 0; i < 245; ++i) {
    if (!yoru_dynarray_append(&arr, i)) {
      printf("failed to append at index %zu, exiting...\n", i);
      return 1;
    }
  }

  print_dynarray(arr);
  return 0;
}

void print_dynarray(IntDynArray arr) {
  printf("IntDynArray {\n");

  printf("  items = %p;\n", arr.__da_items__);
  printf("  size = %zu;\n", arr.__da_size__);
  printf("  capacity = %zu;\n", arr.__da_capacity__);

  printf("}\n");
}
