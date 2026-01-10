#define YORU_IMPL
#define YORU_INCLUDE_EXPERIMENTAL
#include "../yoru.h"

#include <stdio.h>
#include <stddef.h>

int main() {
  yoru_install_crashhandler();

  char *null_ptr = NULL;
  printf("%c\n", *null_ptr);
  return 0;
}
