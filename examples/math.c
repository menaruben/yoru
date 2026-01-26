#define YORU_IMPL
#include "../yoru.h"

#include <stdio.h>

int main() {
  Yoru_Vec3_I64 v1 = yoru_vec3_i64_make(1, 2, 3);
  Yoru_Vec3_I64 v2 = yoru_vec3_i64_make(4, 5, 6);

  Yoru_Vec3_I64 res = {0};
  yoru_vec_add(&v1, &v2, &res);

  
  printf("Vec3{.x = %li, .y = %li, .z = %li}\n", res.elements[0], res.elements[1], res.elements[2]);

  i64 min = 2;
  i64 max = 5;

  Yoru_Vec3_I64 res2 = {0};
  yoru_vec_clamp(&res, min, max, &res2);
  printf("Vec3{.x = %li, .y = %li, .z = %li}\n", res2.elements[0], res2.elements[1], res2.elements[2]);

  usize res3 = 0;
  yoru_vec_length_squared(&res2, &res3);
  printf("length = %zu\n", res3);

  return 0;
}
