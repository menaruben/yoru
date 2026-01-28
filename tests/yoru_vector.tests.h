#ifndef __YORU_VEC_TESTS_H__
#define __YORU_VEC_TESTS_H__

#include <stddef.h>

#include "../yoru.h"
#include "yoru_test_helpers.h"

/* ============================================================
   MODULE: Vector
   ============================================================ */

bool yoru_vec_add_test() {
  Yoru_Vec3_F64 a   = yoru_vec_make(1.0, 2.0, 3.0);
  Yoru_Vec3_F64 b   = yoru_vec_make(4.0, 5.0, 6.0);
  Yoru_Vec3_F64 out = {0};
  Yoru_VecErr   err;

  err = yoru_vec3_add(&a, &b, &out);

  YORU_EXPECT_TRUE(out.elements[0] == 5.0);
  YORU_EXPECT_TRUE(out.elements[1] == 7.0);
  YORU_EXPECT_TRUE(out.elements[2] == 9.0);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_sub_test() {
  Yoru_Vec3_F64 a   = yoru_vec_make(10.0, 8.0, 6.0);
  Yoru_Vec3_F64 b   = yoru_vec_make(1.0, 2.0, 3.0);
  Yoru_Vec3_F64 out = {0};
  Yoru_VecErr   err;

  err = yoru_vec3_sub(&a, &b, &out);

  YORU_EXPECT_TRUE(out.elements[0] == 9.0);
  YORU_EXPECT_TRUE(out.elements[1] == 6.0);
  YORU_EXPECT_TRUE(out.elements[2] == 3.0);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_mul_test() {
  Yoru_Vec3_F64 a   = yoru_vec_make(2.0, 3.0, 4.0);
  Yoru_Vec3_F64 b   = yoru_vec_make(5.0, 6.0, 7.0);
  Yoru_Vec3_F64 out = {0};
  Yoru_VecErr   err;

  err = yoru_vec3_mul(&a, &b, &out);

  YORU_EXPECT_TRUE(out.elements[0] == 10.0);
  YORU_EXPECT_TRUE(out.elements[1] == 18.0);
  YORU_EXPECT_TRUE(out.elements[2] == 28.0);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_div_test() {
  Yoru_Vec3_F64 a   = yoru_vec_make(10.0, 20.0, 30.0);
  Yoru_Vec3_F64 b   = yoru_vec_make(2.0, 4.0, 5.0);
  Yoru_Vec3_F64 out = {0};
  Yoru_VecErr   err;

  err = yoru_vec3_div(&a, &b, &out);

  YORU_EXPECT_TRUE(out.elements[0] == 5.0);
  YORU_EXPECT_TRUE(out.elements[1] == 5.0);
  YORU_EXPECT_TRUE(out.elements[2] == 6.0);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_dot_test() {
  Yoru_Vec3_F64 a   = yoru_vec_make(1.0, 2.0, 3.0);
  Yoru_Vec3_F64 b   = yoru_vec_make(4.0, 5.0, 6.0);
  f64           out = 0;
  Yoru_VecErr   err;

  err = yoru_vec3_dot(&a, &b, &out);

  YORU_EXPECT_TRUE(out == 32.0); // 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_scale_test() {
  Yoru_Vec3_F64 a   = yoru_vec_make(2.0, 3.0, 4.0);
  Yoru_Vec3_F64 out = {0};
  Yoru_VecErr   err;

  err = yoru_vec3_scale(&a, 3.0, &out);

  YORU_EXPECT_TRUE(out.elements[0] == 6.0);
  YORU_EXPECT_TRUE(out.elements[1] == 9.0);
  YORU_EXPECT_TRUE(out.elements[2] == 12.0);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_negate_test() {
  Yoru_Vec3_F64 a   = yoru_vec_make(1.0, -2.0, 3.0);
  Yoru_Vec3_F64 out = {0};
  Yoru_VecErr   err;

  // Negate using scale with -1
  err = yoru_vec3_scale(&a, -1.0, &out);

  YORU_EXPECT_TRUE(out.elements[0] == -1.0);
  YORU_EXPECT_TRUE(out.elements[1] == 2.0);
  YORU_EXPECT_TRUE(out.elements[2] == -3.0);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_length_squared_test() {
  Yoru_Vec3_F64 a   = yoru_vec_make(3.0, 4.0, 0.0);
  f64           out = 0;
  Yoru_VecErr   err;

  err = yoru_vec3_length_squared(&a, &out);

  YORU_EXPECT_TRUE(out == 25.0); // 3^2 + 4^2 + 0^2 = 9 + 16 = 25
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_min_test() {
  Yoru_Vec3_F64 a   = yoru_vec_make(1.0, 5.0, 3.0);
  Yoru_Vec3_F64 b   = yoru_vec_make(2.0, 3.0, 4.0);
  Yoru_Vec3_F64 out = {0};
  Yoru_VecErr   err;

  err = yoru_vec3_min_between(&a, &b, &out);

  YORU_EXPECT_TRUE(out.elements[0] == 1.0);
  YORU_EXPECT_TRUE(out.elements[1] == 3.0);
  YORU_EXPECT_TRUE(out.elements[2] == 3.0);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_max_test() {
  Yoru_Vec3_F64 a   = yoru_vec_make(1.0, 5.0, 3.0);
  Yoru_Vec3_F64 b   = yoru_vec_make(2.0, 3.0, 4.0);
  Yoru_Vec3_F64 out = {0};
  Yoru_VecErr   err;

  err = yoru_vec3_max_between(&a, &b, &out);

  YORU_EXPECT_TRUE(out.elements[0] == 2.0);
  YORU_EXPECT_TRUE(out.elements[1] == 5.0);
  YORU_EXPECT_TRUE(out.elements[2] == 4.0);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec3_cross_test() {
  Yoru_Vec3_F64 a   = yoru_vec_make(1.0, 0.0, 0.0);
  Yoru_Vec3_F64 b   = yoru_vec_make(0.0, 1.0, 0.0);
  Yoru_Vec3_F64 out = {0};
  Yoru_VecErr   err;

  err = yoru_vec3_cross(&a, &b, &out);

  YORU_EXPECT_TRUE(out.elements[0] == 0.0);
  YORU_EXPECT_TRUE(out.elements[1] == 0.0);
  YORU_EXPECT_TRUE(out.elements[2] == 1.0); // i × j = k
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_null_pointer_test() {
  Yoru_Vec3_F64 a   = yoru_vec_make(1.0, 2.0, 3.0);
  Yoru_Vec3_F64 out = {0};
  Yoru_VecErr   err;

  Yoru_Vec3_F64 *nilptr = NULL;

  err = yoru_vec3_add(nilptr, &a, &out);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_NULL);

  err = yoru_vec3_add(&a, nilptr, &out);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_NULL);

  err = yoru_vec3_add(&a, &a, nilptr);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_NULL);

  return true;

err:
  return false;
}

#endif
