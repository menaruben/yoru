#define YORU_IMPL
#include "../src/yoru.h"

#include <assert.h>
#include <stdio.h>

typedef struct {
  Yoru_ArrayList_T(int) v;
} IntArr;

typedef struct {
  Yoru_ArrayList_T(int *) v;
} IntPtrArr;

#define print_array(__arr)                                                                                             \
  do {                                                                                                                 \
    printf("size: %zu, capacity: %zu\n", __arr.v.size, __arr.v.capacity);                                              \
    for (usize i = 0; i < __arr.v.size; ++i) {                                                                         \
      printf("[%zu]: %d\n", i, __arr.v.items[i]);                                                                      \
    }                                                                                                                  \
  } while (0)

int main() {
  Yoru_GlobalAllocator allocator = yoru_global_allocator_make();
  IntArr               varr      = {0};
  IntPtrArr            parr      = {0};

  printf("before init:\n");
  print_array(varr);
  print_array(parr);

  
  yoru_arraylist_init(&varr.v, &allocator);
  yoru_arraylist_init(&parr.v, &allocator);
  printf("after init:\n");
  print_array(varr);
  print_array(parr);

  printf("appending values...\n");
  usize i = 0;
  for (; i < 49; ++i) {
    yoru_arraylist_append(&varr.v, i);
    yoru_arraylist_append(&parr.v, (int *)&i);
  }

  print_array(varr);
  print_array(parr);

  int *x = parr.v.items[0];
  printf("%p = %d\n", x, *x);


  printf("resizing arraylists to capcacity 14");
  yoru_arraylist_resize(&varr.v, 14);
  yoru_arraylist_resize(&parr.v, 14);

  print_array(varr);
  print_array(parr);
    
  return 0;
}
