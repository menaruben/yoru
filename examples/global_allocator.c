#define YORU_IMPL
#include "../src/yoru.h"

#include <assert.h>
#include <stdio.h>

struct Person {
  const char *name;
  usize age;
};

int main() {
  Yoru_GlobalAllocator allocator = yoru_global_allocator_make();

  Yoru_Opt ptr = yoru_allocator_alloc(&allocator, sizeof(struct Person));
  assert(ptr.has_value);
  assert(ptr.ptr);

  auto p = (struct Person *)ptr.ptr;
  p->age = 22;
  p->name = "ruby";

  printf("name: %s\n", p->name);
  printf("age: %zu\n", p->age);

  yoru_allocator_free(&allocator, p);
  return 0;
}
