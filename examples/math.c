#define YORU_IMPL
#include "../src/yoru.h"

#include <stdio.h>

int main() {
  printf("ln(1.0) = %f\n", yoru_ln(1.0));
  printf("ln(e) = %f\n", yoru_ln(YORU_EULER));
  printf("ln(2.0) = %f\n", yoru_ln(2.0));
  printf("ln(10.0) = %f\n", yoru_ln(10.0));
  printf("ln(0.75) = %f\n", yoru_ln(0.75));

  printf("log24.5(23451.52) = %f\n", yoru_log(23451.52, 24.5));
  return 0;
}
