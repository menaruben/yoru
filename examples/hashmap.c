#define YORU_IMPL
#include "../yoru.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  const char *name;
  usize       age;
} Person;

typedef Yoru_HashMap_T(Person) Persons;

const char *names[] = {
    "Denji",  "Makima", "Power",  "Aki",    "Himeno",   "Eren",    "Mikasa",  "Armin",    "Levi",    "Hange",
    "Yuji",   "Megumi", "Nobara", "Gojo",   "Kazuma",   "Aqua",    "Megumin", "Darkness", "Light",   "L",
    "Ryuk",   "Misa",   "Near",   "Mello",  "Historia", "Reiner",  "Bertolt", "Annie",    "Zeke",    "Satoru",
    "Nobara", "Megumi", "Yuji",   "Suguru", "Mai",      "Tanjiro", "Nezuko",  "Zenitsu",  "Inosuke", "Giyu",
};

usize name_count = sizeof(names) / sizeof(names[0]);
#define NUM_PERSON 20

int main() {
  Yoru_GlobalAllocator allocator = yoru_global_allocator_make();
  Persons              persons   = {0};
  yoru_hashmap_init(&persons, &allocator);

  for (usize i = 0; i < NUM_PERSON; ++i) {
    const char *name = names[i];
    Person      p    = {.name = name, .age = (rand() % 51) + 10};
    yoru_hashmap_set(&persons, p.name, p);
  }

  printf("HashMap{items: %p, size: %zu, capacity: %zu}\n", persons.entries.items, persons.entries.size, persons.entries.capacity);
  for (usize i = 0; i < persons.keys.size; ++i) {
    cstr   key = persons.keys.items[i].key;
    Person p   = {0};
    yoru_hashmap_get(&persons, key, &p);
    printf("Person{name: %s, %zu}\n", p.name, p.age);
  }

  yoru_hashmap_destroy(&persons);
  return 0;
}
