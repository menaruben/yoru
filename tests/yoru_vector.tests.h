#ifndef __YORU_VEC_TESTS_H__
#define __YORU_VEC_TESTS_H__

#include <stddef.h>

#include "../yoru.h"
#include "yoru_test_helpers.h"

/* ============================================================
   MODULE: Vector
   ============================================================ */

bool yoru_vec_add_test() {
  Yoru_Vec3_I64 a   = yoru_vec_make(1, 2, 3);
  Yoru_Vec3_I64 b   = yoru_vec_make(4, 5, 6);
  Yoru_Vec3_I64 out = {0};
  Yoru_VecErr   err = YORU_VEC_ERR_OK;

  yoru_vec_add(&a, &b, &out, &err);

  YORU_EXPECT_TRUE(out.elements[0] == 5);
  YORU_EXPECT_TRUE(out.elements[1] == 7);
  YORU_EXPECT_TRUE(out.elements[2] == 9);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_sub_test() {
  Yoru_Vec3_I64 a   = yoru_vec_make(10, 8, 6);
  Yoru_Vec3_I64 b   = yoru_vec_make(1, 2, 3);
  Yoru_Vec3_I64 out = {0};
  Yoru_VecErr   err = YORU_VEC_ERR_OK;

  yoru_vec_sub(&a, &b, &out, &err);

  YORU_EXPECT_TRUE(out.elements[0] == 9);
  YORU_EXPECT_TRUE(out.elements[1] == 6);
  YORU_EXPECT_TRUE(out.elements[2] == 3);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_mul_test() {
  Yoru_Vec3_I64 a   = yoru_vec_make(2, 3, 4);
  Yoru_Vec3_I64 b   = yoru_vec_make(5, 6, 7);
  Yoru_Vec3_I64 out = {0};
  Yoru_VecErr   err = YORU_VEC_ERR_OK;

  yoru_vec_mul(&a, &b, &out, &err);

  YORU_EXPECT_TRUE(out.elements[0] == 10);
  YORU_EXPECT_TRUE(out.elements[1] == 18);
  YORU_EXPECT_TRUE(out.elements[2] == 28);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_div_test() {
  Yoru_Vec3_I64 a   = yoru_vec_make(10, 20, 30);
  Yoru_Vec3_I64 b   = yoru_vec_make(2, 4, 5);
  Yoru_Vec3_I64 out = {0};
  Yoru_VecErr   err = YORU_VEC_ERR_OK;

  yoru_vec_div(&a, &b, &out, &err);

  YORU_EXPECT_TRUE(out.elements[0] == 5);
  YORU_EXPECT_TRUE(out.elements[1] == 5);
  YORU_EXPECT_TRUE(out.elements[2] == 6);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_dot_test() {
  Yoru_Vec3_I64 a   = yoru_vec_make(1, 2, 3);
  Yoru_Vec3_I64 b   = yoru_vec_make(4, 5, 6);
  i64           out = 0;
  Yoru_VecErr   err = YORU_VEC_ERR_OK;

  yoru_vec_dot(&a, &b, out, &err);

  YORU_EXPECT_TRUE(out == 32); // 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_scale_test() {
  Yoru_Vec3_I64 a   = yoru_vec_make(2, 3, 4);
  Yoru_Vec3_I64 out = {0};
  Yoru_VecErr   err = YORU_VEC_ERR_OK;

  yoru_vec_scale(&a, 3, &out, &err);

  YORU_EXPECT_TRUE(out.elements[0] == 6);
  YORU_EXPECT_TRUE(out.elements[1] == 9);
  YORU_EXPECT_TRUE(out.elements[2] == 12);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_negate_test() {
  Yoru_Vec3_I64 a   = yoru_vec_make(1, -2, 3);
  Yoru_Vec3_I64 out = {0};
  Yoru_VecErr   err = YORU_VEC_ERR_OK;

  yoru_vec_negate(&a, &out, &err);

  YORU_EXPECT_TRUE(out.elements[0] == -1);
  YORU_EXPECT_TRUE(out.elements[1] == 2);
  YORU_EXPECT_TRUE(out.elements[2] == -3);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_length_squared_test() {
  Yoru_Vec3_I64 a   = yoru_vec_make(3, 4, 0);
  i64           out = 0;
  Yoru_VecErr   err = YORU_VEC_ERR_OK;

  yoru_vec_length_squared(&a, &out, &err);

  YORU_EXPECT_TRUE(out == 25); // 3^2 + 4^2 + 0^2 = 9 + 16 = 25
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_min_test() {
  Yoru_Vec3_I64 a   = yoru_vec_make(1, 5, 3);
  Yoru_Vec3_I64 b   = yoru_vec_make(2, 3, 4);
  Yoru_Vec3_I64 out = {0};
  Yoru_VecErr   err = YORU_VEC_ERR_OK;

  yoru_vec_min(&a, &b, &out, &err);

  YORU_EXPECT_TRUE(out.elements[0] == 1);
  YORU_EXPECT_TRUE(out.elements[1] == 3);
  YORU_EXPECT_TRUE(out.elements[2] == 3);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_max_test() {
  Yoru_Vec3_I64 a   = yoru_vec_make(1, 5, 3);
  Yoru_Vec3_I64 b   = yoru_vec_make(2, 3, 4);
  Yoru_Vec3_I64 out = {0};
  Yoru_VecErr   err = YORU_VEC_ERR_OK;

  yoru_vec_max(&a, &b, &out, &err);

  YORU_EXPECT_TRUE(out.elements[0] == 2);
  YORU_EXPECT_TRUE(out.elements[1] == 5);
  YORU_EXPECT_TRUE(out.elements[2] == 4);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec3_cross_test() {
  Yoru_Vec3_I64 a   = yoru_vec_make(1, 0, 0);
  Yoru_Vec3_I64 b   = yoru_vec_make(0, 1, 0);
  Yoru_Vec3_I64 out = {0};
  Yoru_VecErr   err = YORU_VEC_ERR_OK;

  yoru_vec3_cross(&a, &b, &out, &err);

  YORU_EXPECT_TRUE(out.elements[0] == 0);
  YORU_EXPECT_TRUE(out.elements[1] == 0);
  YORU_EXPECT_TRUE(out.elements[2] == 1); // i × j = k
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_OK);

  return true;

err:
  return false;
}

bool yoru_vec_null_pointer_test() {
  Yoru_Vec3_I64 a   = yoru_vec_make(1, 2, 3);
  Yoru_Vec3_I64 out = {0};
  Yoru_VecErr   err = YORU_VEC_ERR_OK;

  Yoru_Vec3_I64 *nilptr = NULL;

  yoru_vec_add(nilptr, &a, &out, &err);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_NULL);

  err = YORU_VEC_ERR_OK;
  yoru_vec_add(&a, nilptr, &out, &err);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_NULL);

  err = YORU_VEC_ERR_OK;
  yoru_vec_add(&a, &a, nilptr, &err);
  YORU_EXPECT_TRUE(err == YORU_VEC_ERR_NULL);

  return true;

err:
  return false;
}

#endif
