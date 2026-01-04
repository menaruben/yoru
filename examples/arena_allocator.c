#define YORU_IMPL
#include "../src/yoru.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Person {
  const char *name;
  usize       age;
};

const char *names[] = {
    "Denji",
    "Makima",
    "Power",
    "Aki",
    "Himeno",
    "Eren",
    "Mikasa",
    "Armin",
    "Levi",
    "Hange",
    "Yuji",
    "Megumi",
    "Nobara",
    "Gojo",
};

usize name_count = sizeof(names) / sizeof(names[0]);

#define NUM_PERSON (10)

int main() {
  srand((unsigned)time(NULL));
  Yoru_ArenaAllocator *allocator = yoru_arena_allocator_make(NUM_PERSON * sizeof(struct Person));

  struct Person *people[10];

  for (usize i = 0; i < NUM_PERSON; i++) {
    Yoru_Opt ptr = yoru_allocator_alloc(allocator, sizeof(struct Person));

    assert(ptr.has_value);
    assert(ptr.ptr);

    struct Person *p = ptr.ptr;
    people[i]        = p;

    p->name = names[rand() % name_count];
    p->age  = (rand() % 51) + 10;
  }

  for (usize i = 0; i < NUM_PERSON; i++) {
    printf("Person %zu -> name: %-10s age: %zu\n", i, people[i]->name, people[i]->age);
  }

  // now the arena should be full so we can *try* to push another person onto the arena
  // and it should return an empty optional
  Yoru_Opt optr = yoru_allocator_alloc(allocator, sizeof(struct Person));
  assert(!optr.has_value);
  printf("arena full, attempted push returned empty optional (expected)\n");

  // thats a no-op, to free the memory of the allocator in the case of an arena you call the destructor
  // with yoru_allocator_destroy (see below)
  // for (usize i = 0; i < NUM_PERSON; i++) {
  //   yoru_allocator_free(allocator, people[i]);
  // }

  yoru_allocator_destroy(allocator);
  return 0;
}
