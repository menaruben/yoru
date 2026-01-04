#define YORU_IMPL
#include "../src/yoru.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

Yoru_Opt get_maybe_int();

int main() {
  srand(time(NULL));
  auto maybe_int = get_maybe_int();

  if (maybe_int.has_value) {
    assert(maybe_int.ptr);
    int value = *(int *)maybe_int.ptr;
    printf("optional has value: %d\n", value);
    free(maybe_int.ptr);
    return 0;
  }

  assert(!maybe_int.ptr && "maybe int has no value but ptr is not nullptr");
  printf("optional has no value\n");
  return 0;
}

Yoru_Opt get_maybe_int() {
  int x = rand() % 100;

  if (x < 80) {
    int *x = malloc(sizeof(int));
    assert(x && "failed to alloc mem for x");
    *x = 69;
    return yoru_opt_some(x);
  }

  return yoru_opt_none();
}
