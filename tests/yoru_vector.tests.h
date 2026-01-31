#ifndef __YORU_VEC_TESTS_H__
#define __YORU_VEC_TESTS_H__

#include <stddef.h>
#include <stdio.h>

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

bool yoru_mat_transpose_test() {
  usize           nrows          = 3;
  usize           ncols          = 3;
  Yoru_Mat3x3_F64 mat            = yoru_mat_make(nrows, ncols, 1, 2, 3, 4, 5, 6, 7, 8, 9);
  Yoru_Mat3x3_F64 mat_t_expected = yoru_mat_make(nrows, ncols, 1, 4, 7, 2, 5, 8, 3, 6, 9);
  Yoru_Mat3x3_F64 mat_t_actual   = {0};
  mat_t_actual.ncols             = ncols;
  mat_t_actual.nrows             = nrows;

  Yoru_MatErr err = yoru_mat_transpose(3, 3, mat.elements, mat_t_actual.elements);
  YORU_EXPECT_TRUE(err = YORU_MAT_ERR_OK);
  YORU_EXPECT_EQ_MEM(mat_t_expected.elements, mat_t_actual.elements, ncols * nrows);
  return true;

err:
  return false;
}

bool yoru_mat_mul_test_quadratic_mat() {
  usize           nrows            = 3;
  usize           ncols            = 3;
  Yoru_Mat3x3_F64 mat1             = yoru_mat_make(nrows, ncols, 1, 2, 3, 4, 5, 6, 7, 8, 9);
  Yoru_Mat3x3_F64 mat2             = yoru_mat_make(nrows, ncols, 1, 4, 7, 2, 5, 8, 3, 6, 9);
  Yoru_Mat3x3_F64 mat_res_expected = yoru_mat_make(nrows, ncols, 66, 78, 90, 78, 93, 108, 90, 108, 126);
  Yoru_Mat3x3_F64 mat_res_actual   = {0};
  mat_res_actual.ncols             = ncols;
  mat_res_actual.nrows             = nrows;

  Yoru_MatErr merr = yoru_mat_mul(nrows, nrows, mat1.elements, nrows, ncols, mat2.elements, mat_res_actual.elements);
  YORU_EXPECT_TRUE(merr = YORU_MAT_ERR_OK);
  YORU_EXPECT_EQ_MEM(mat_res_expected.elements, mat_res_actual.elements, nrows * ncols);
  return true;
err:
  return false;
}

bool yoru_mat_mul_test() {
  usize nrows1             = 2;
  usize ncols1             = 4;
  usize nrows2             = 4;
  usize ncols2             = 3;
  f64   mat1[]             = {1, 2, 3, 4, 5, 6, 7, 8};
  f64   mat2[]             = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
  f64   mat_res_expected[] = {50, 60, 114, 140, 178, 220};
  f64   mat_res_actual[6]  = {0};

  Yoru_MatErr merr = yoru_mat_mul(nrows1, ncols1, mat1, nrows2, ncols2, mat2, mat_res_actual);
  YORU_EXPECT_TRUE(merr == YORU_MAT_ERR_OK);
  YORU_EXPECT_EQ_MEM(mat_res_expected, mat_res_actual, nrows1 * ncols2);
  return true;
err:
  return false;
}

bool yoru_mat_identity_test() {
  usize n                       = 3;
  f64   identity_mat_exp[3 * 3] = {0};
  identity_mat_exp[0]           = 1;
  identity_mat_exp[4]           = 1;
  identity_mat_exp[8]           = 1;

  f64         identity_mat_act[3 * 3] = {0};
  Yoru_MatErr merr                    = yoru_mat_identity(n, identity_mat_act);
  YORU_EXPECT_TRUE(merr == YORU_MAT_ERR_OK);
  YORU_EXPECT_EQ_MEM(identity_mat_exp, identity_mat_act, n * n);
  return true;
err:
  return false;
}

#endif
