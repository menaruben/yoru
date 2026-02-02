#define YORU_USE_SIMD
#define YORU_IMPL
#include "../yoru.h"
#include <immintrin.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

const int ITERATIONS = 4000000;

void vec_naive_dot(usize n, const f64 *v1, const f64 *v2, f64 *res) {
  for (usize i = 0; i < n; ++i)
    *res += v1[i] * v2[i];
}

void vec_simd_dot(usize n, const f64 *v1, const f64 *v2, f64 *res) {
#if defined(__AVX__)
  usize i      = 0;
  f64  *temp_v = calloc(n, sizeof(f64));
  assert(temp_v);
  for (; i + 4 <= n; i += 4) {
    __m256d a = _mm256_loadu_pd(&v1[i]);
    __m256d b = _mm256_loadu_pd(&v2[i]);
    _mm256_storeu_pd(&temp_v[i], _mm256_mul_pd(a, b));
  }
  for (; i < n; ++i)
    temp_v[i] = v1[i] * v2[i];

  for (usize x = 0; x < n; ++x)
    *res += temp_v[x];
  free(temp_v);
#else
  // vec_naive_dot(n, v1, v2, res);
  assert(false);
#endif
}

void vec_yoru_dot(usize n, const f64 *v1, const f64 *v2, f64 *res) {
  assert(yoru_vec_dot(n, v1, v2, res) == YORU_VEC_ERR_OK);
}

// ---------------- Benchmarking ----------------

double time_vec_dot(usize n, void (*dot_func)(usize, const f64 *, const f64 *, f64 *)) {
  f64 *v1  = malloc(sizeof(f64) * n);
  f64 *v2  = malloc(sizeof(f64) * n);
  f64  res = 0;

  if (!v1 || !v2) {
    fprintf(stderr, "allocation failed\n");
    exit(1);
  }

  // Prefill once (NOT timed)
  for (usize i = 0; i < n; ++i) {
    v1[i] = (double)(rand() % 1000) / 10.0;
    v2[i] = (double)(rand() % 1000) / 10.0;
  }

  clock_t start = clock();

  for (int iter = 0; iter < ITERATIONS; ++iter) {
    dot_func(n, v1, v2, &res);
  }

  clock_t end = clock();

  free(v1);
  free(v2);

  return (double)(end - start) / CLOCKS_PER_SEC;
}

// ---------------- Main ----------------

int main() {
  srand((unsigned int)time(NULL));

  printf("=== Timing Vec2 ===\n");
  printf("SIMD:  %f sec\n", time_vec_dot(2, vec_simd_dot));
  printf("Naive: %f sec\n", time_vec_dot(2, vec_naive_dot));
  printf("Yoru:  %f sec\n", time_vec_dot(2, vec_yoru_dot));

  printf("\n=== Timing Vec3 ===\n");
  printf("SIMD:  %f sec\n", time_vec_dot(3, vec_simd_dot));
  printf("Naive: %f sec\n", time_vec_dot(3, vec_naive_dot));
  printf("Yoru:  %f sec\n", time_vec_dot(3, vec_yoru_dot));

  printf("\n=== Timing Vec4 ===\n");
  printf("SIMD:  %f sec\n", time_vec_dot(4, vec_simd_dot));
  printf("Naive: %f sec\n", time_vec_dot(4, vec_naive_dot));
  printf("Yoru:  %f sec\n", time_vec_dot(4, vec_yoru_dot));

  printf("\n=== Timing Vec10 ===\n");
  printf("SIMD:  %f sec\n", time_vec_dot(10, vec_simd_dot));
  printf("Naive: %f sec\n", time_vec_dot(10, vec_naive_dot));
  printf("Yoru:  %f sec\n", time_vec_dot(10, vec_yoru_dot));

  printf("\n=== Timing Vec20 ===\n");
  printf("SIMD:  %f sec\n", time_vec_dot(20, vec_simd_dot));
  printf("Naive: %f sec\n", time_vec_dot(20, vec_naive_dot));
  printf("Yoru:  %f sec\n", time_vec_dot(20, vec_yoru_dot));

  printf("\n=== Timing Vec256 ===\n");
  printf("SIMD:  %f sec\n", time_vec_dot(256, vec_simd_dot));
  printf("Naive: %f sec\n", time_vec_dot(256, vec_naive_dot));
  printf("Yoru:  %f sec\n", time_vec_dot(256, vec_yoru_dot));

  printf("\n=== Timing Vec2048 ===\n");
  printf("SIMD:  %f sec\n", time_vec_dot(2048, vec_simd_dot));
  printf("Naive: %f sec\n", time_vec_dot(2048, vec_naive_dot));
  printf("Yoru:  %f sec\n", time_vec_dot(2048, vec_yoru_dot));

  printf("\n=== Timing Vec8192 ===\n");
  printf("SIMD:  %f sec\n", time_vec_dot(8192, vec_simd_dot));
  printf("Naive: %f sec\n", time_vec_dot(8192, vec_naive_dot));
  printf("Yoru:  %f sec\n", time_vec_dot(8192, vec_yoru_dot));

  printf("\n=== Timing Vec16384 ===\n");
  printf("SIMD:  %f sec\n", time_vec_dot(16384, vec_simd_dot));
  printf("Naive: %f sec\n", time_vec_dot(16384, vec_naive_dot));
  printf("Yoru:  %f sec\n", time_vec_dot(16384, vec_yoru_dot));

  printf("\n=== Timing Vec32768 ===\n");
  printf("SIMD:  %f sec\n", time_vec_dot(32768, vec_simd_dot));
  printf("Naive: %f sec\n", time_vec_dot(32768, vec_naive_dot));
  printf("Yoru:  %f sec\n", time_vec_dot(32768, vec_yoru_dot));

  return 0;
}
