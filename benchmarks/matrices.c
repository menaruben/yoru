#define YORU_USE_SIMD
#define YORU_IMPL
#include "../yoru.h"
#include <immintrin.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

int get_iterations(usize size) {
  if (size <= 10) return 100000;
  if (size <= 50) return 10000;
  if (size <= 100) return 1000;
  if (size <= 500) return 100;
  if (size <= 1000) return 10;
  if (size <= 5000) return 3;
  return 1;
}

void mat_naive_mul(usize nrows1, usize ncols1, f64 *mat1, usize nrows2, usize ncols2, f64 *mat2, f64 *out_mat) {
  for (usize i = 0; i < nrows1; ++i) {
    for (usize j = 0; j < ncols2; ++j) {
      f64 sum = 0.0;
      for (usize k = 0; k < ncols1; ++k) {
        f64 a = mat1[k * nrows1 + i];
        f64 b = mat2[j * nrows2 + k];
        sum += a * b;
      }
      out_mat[j * nrows1 + i] = sum;
    }
  }
}

void mat_yoru_mul(usize nrows1, usize ncols1, f64 *mat1, usize nrows2, usize ncols2, f64 *mat2, f64 *out_mat) {
  Yoru_MatErr err = yoru_mat_mul(nrows1, ncols1, mat1, nrows2, ncols2, mat2, out_mat);
  assert(err == YORU_MAT_ERR_OK);
}

// Prevent compiler from optimizing away the result
volatile f64 g_sink = 0.0;

void use_result(f64 *result, usize size) {
  // Touch a few elements to prevent dead code elimination
  g_sink += result[0];
  if (size > 1) g_sink += result[size - 1];
  if (size > 100) g_sink += result[size / 2];
}

double time_mat_mul(
    usize nrows1,
    usize ncols1,
    usize nrows2,
    usize ncols2,
    void (*mul_func)(usize, usize, f64 *, usize, usize, f64 *, f64 *)) {
  assert(ncols1 == nrows2);

  f64 *mat1 = malloc(sizeof(f64) * nrows1 * ncols1);
  f64 *mat2 = malloc(sizeof(f64) * nrows2 * ncols2);
  f64 *res  = malloc(sizeof(f64) * nrows1 * ncols2);

  if (!mat1 || !mat2 || !res) {
    fprintf(stderr, "allocation failed\n");
    exit(1);
  }

  // Prefill once (NOT timed)
  for (usize i = 0; i < nrows1 * ncols1; ++i)
    mat1[i] = (double)(rand() % 1000) / 10.0;
  for (usize i = 0; i < nrows2 * ncols2; ++i)
    mat2[i] = (double)(rand() % 1000) / 10.0;

  int iterations = get_iterations(nrows1 > ncols2 ? nrows1 : ncols2);

  clock_t start = clock();
  for (int iter = 0; iter < iterations; ++iter) {
    mul_func(nrows1, ncols1, mat1, nrows2, ncols2, mat2, res);
    use_result(res, nrows1 * ncols2);
  }
  clock_t end = clock();

  free(mat1);
  free(mat2);
  free(res);

  double total_time = (double)(end - start) / CLOCKS_PER_SEC;
  return total_time / iterations;
}

int main() {
  srand((unsigned int)time(NULL));

  printf("=== Timing 2x3 * 3x2 (square-ish, small) ===\n");
  printf("Naive: %f sec\n", time_mat_mul(2, 3, 3, 2, mat_naive_mul));
  printf("Yoru:  %f sec\n", time_mat_mul(2, 3, 3, 2, mat_yoru_mul));

  printf("\n=== Timing 3x3 * 3x3 (square, small) ===\n");
  printf("Naive: %f sec\n", time_mat_mul(3, 3, 3, 3, mat_naive_mul));
  printf("Yoru:  %f sec\n", time_mat_mul(3, 3, 3, 3, mat_yoru_mul));

  printf("\n=== Timing 4x4 * 4x4 (square, small) ===\n");
  printf("Naive: %f sec\n", time_mat_mul(4, 4, 4, 4, mat_naive_mul));
  printf("Yoru:  %f sec\n", time_mat_mul(4, 4, 4, 4, mat_yoru_mul));

  printf("\n=== Timing 10x20 * 20x10 (non-square, small) ===\n");
  printf("Naive: %f sec\n", time_mat_mul(10, 20, 20, 10, mat_naive_mul));
  printf("Yoru:  %f sec\n", time_mat_mul(10, 20, 20, 10, mat_yoru_mul));

  printf("\n=== Timing 50x50 * 50x50 (square, medium) ===\n");
  printf("Naive: %f sec\n", time_mat_mul(50, 50, 50, 50, mat_naive_mul));
  printf("Yoru:  %f sec\n", time_mat_mul(50, 50, 50, 50, mat_yoru_mul));

  printf("\n=== Timing 100x50 * 50x100 (non-square, medium) ===\n");
  printf("Naive: %f sec\n", time_mat_mul(100, 50, 50, 100, mat_naive_mul));
  printf("Yoru:  %f sec\n", time_mat_mul(100, 50, 50, 100, mat_yoru_mul));

  printf("\n=== Timing 100x100 * 100x100 (square, medium) ===\n");
  printf("Naive: %f sec\n", time_mat_mul(100, 100, 100, 100, mat_naive_mul));
  printf("Yoru:  %f sec\n", time_mat_mul(100, 100, 100, 100, mat_yoru_mul));

  printf("\n=== Timing 256x128 * 128x256 (non-square, medium-large) ===\n");
  printf("Naive: %f sec\n", time_mat_mul(256, 128, 128, 256, mat_naive_mul));
  printf("Yoru:  %f sec\n", time_mat_mul(256, 128, 128, 256, mat_yoru_mul));

  printf("\n=== Timing 500x500 * 500x500 (square, large) ===\n");
  printf("Naive: %f sec\n", time_mat_mul(500, 500, 500, 500, mat_naive_mul));
  printf("Yoru:  %f sec\n", time_mat_mul(500, 500, 500, 500, mat_yoru_mul));

  printf("\n=== Timing 1000x500 * 500x1000 (non-square, large) ===\n");
  printf("Naive: %f sec\n", time_mat_mul(1000, 500, 500, 1000, mat_naive_mul));
  printf("Yoru:  %f sec\n", time_mat_mul(1000, 500, 500, 1000, mat_yoru_mul));

  printf("\n=== Timing 1000x1000 * 1000x1000 (square, very large) ===\n");
  printf("Naive: %f sec\n", time_mat_mul(1000, 1000, 1000, 1000, mat_naive_mul));
  printf("Yoru:  %f sec\n", time_mat_mul(1000, 1000, 1000, 1000, mat_yoru_mul));

  printf("\n=== Timing 2048x1024 * 1024x2048 (non-square, very large) ===\n");
  printf("Naive: %f sec\n", time_mat_mul(2048, 1024, 1024, 2048, mat_naive_mul));
  printf("Yoru:  %f sec\n", time_mat_mul(2048, 1024, 1024, 2048, mat_yoru_mul));

  printf("\n=== Timing 4096x2048 * 2048x4096 (non-square, huge) ===\n");
  // printf("Naive: %f sec\n", time_mat_mul(4096, 2048, 2048, 4096, mat_naive_mul));
  printf("Yoru:  %f sec\n", time_mat_mul(4096, 2048, 2048, 4096, mat_yoru_mul));

  printf("\n=== Timing 8192x4096 * 4096x8192 (non-square, massive) ===\n");
  // printf("Naive: %f sec\n", time_mat_mul(8192, 4096, 4096, 8192, mat_naive_mul));
  printf("Yoru:  %f sec\n", time_mat_mul(8192, 4096, 4096, 8192, mat_yoru_mul));

  printf("\n=== Timing 100x10000 * 10000x100 (tall x wide) ===\n");
  // printf("Naive: %f sec\n", time_mat_mul(100, 10000, 10000, 100, mat_naive_mul));
  printf("Yoru:  %f sec\n", time_mat_mul(100, 10000, 10000, 100, mat_yoru_mul));

  printf("\n=== Timing 10000x100 * 100x10000 (wide x tall) ===\n");
  // printf("Naive: %f sec\n", time_mat_mul(10000, 100, 100, 10000, mat_naive_mul));
  printf("Yoru:  %f sec\n", time_mat_mul(10000, 100, 100, 10000, mat_yoru_mul));

  // -> commented out naive (should prob rename xD but it took wayyyy to long. felt like i was stuck so i commented them out)

  // Print sink to ensure it's used (prevents whole program optimization)
  printf("\n(sink value: %f)\n", g_sink);

  return 0;
}
