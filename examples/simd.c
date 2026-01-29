#define YORU_USE_SIMD
#define YORU_IMPL
#include "../yoru.h"
#include <immintrin.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

const int ITERATIONS = 4000000;

// Naive addition for arbitrary length
void vec_naive_add(usize n, const f64 *v1, const f64 *v2, f64 *res) {
  for (usize i = 0; i < n; ++i)
    res[i] = v1[i] + v2[i];
}

// SIMD addition for arbitrary length
void vec_simd_add(usize n, const f64 *v1, const f64 *v2, f64 *res) {
#ifdef __AVX__
  usize i = 0;
  for (; i + 4 <= n; i += 4) { // Process 4 doubles at a time
    __m256d a = _mm256_loadu_pd(&v1[i]);
    __m256d b = _mm256_loadu_pd(&v2[i]);
    _mm256_storeu_pd(&res[i], _mm256_add_pd(a, b));
  }
  // Handle remaining elements
  for (; i < n; ++i)
    res[i] = v1[i] + v2[i];
#else
  // fallback to naive if no AVX
  vec_naive_add(n, v1, v2, res);
#endif
}

// Yoru wrapper (uses your core library)
void vec_yoru_add(usize n, const f64 *v1, const f64 *v2, f64 *res) {
  assert(yoru_vec_add(n, v1, v2, res) == YORU_VEC_ERR_OK);
}

// ---------------- Benchmarking ----------------

double time_vec_add(usize n, void (*add_func)(usize, const f64 *, const f64 *, f64 *)) {
  f64 *v1  = malloc(sizeof(f64) * n);
  f64 *v2  = malloc(sizeof(f64) * n);
  f64 *res = malloc(sizeof(f64) * n);

  if (!v1 || !v2 || !res) {
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
    add_func(n, v1, v2, res);
  }

  clock_t end = clock();

  free(v1);
  free(v2);
  free(res);

  return (double)(end - start) / CLOCKS_PER_SEC;
}

// ---------------- Main ----------------

int main() {
  srand((unsigned int)time(NULL));

  printf("=== Timing Vec2 ===\n");
  printf("SIMD:  %f sec\n", time_vec_add(2, vec_simd_add));
  printf("Naive: %f sec\n", time_vec_add(2, vec_naive_add));
  printf("Yoru:  %f sec\n", time_vec_add(2, vec_yoru_add));

  printf("\n=== Timing Vec3 ===\n");
  printf("SIMD:  %f sec\n", time_vec_add(3, vec_simd_add));
  printf("Naive: %f sec\n", time_vec_add(3, vec_naive_add));
  printf("Yoru:  %f sec\n", time_vec_add(3, vec_yoru_add));

  printf("\n=== Timing Vec4 ===\n");
  printf("SIMD:  %f sec\n", time_vec_add(4, vec_simd_add));
  printf("Naive: %f sec\n", time_vec_add(4, vec_naive_add));
  printf("Yoru:  %f sec\n", time_vec_add(4, vec_yoru_add));

  printf("\n=== Timing Vec10 ===\n");
  printf("SIMD:  %f sec\n", time_vec_add(10, vec_simd_add));
  printf("Naive: %f sec\n", time_vec_add(10, vec_naive_add));
  printf("Yoru:  %f sec\n", time_vec_add(10, vec_yoru_add));

  printf("\n=== Timing Vec20 ===\n");
  printf("SIMD:  %f sec\n", time_vec_add(20, vec_simd_add));
  printf("Naive: %f sec\n", time_vec_add(20, vec_naive_add));
  printf("Yoru:  %f sec\n", time_vec_add(20, vec_yoru_add));

  printf("\n=== Timing Vec256 ===\n");
  printf("SIMD:  %f sec\n", time_vec_add(256, vec_simd_add));
  printf("Naive: %f sec\n", time_vec_add(256, vec_naive_add));
  printf("Yoru:  %f sec\n", time_vec_add(256, vec_yoru_add));

  printf("\n=== Timing Vec2048 ===\n");
  printf("SIMD:  %f sec\n", time_vec_add(2048, vec_simd_add));
  printf("Naive: %f sec\n", time_vec_add(2048, vec_naive_add));
  printf("Yoru:  %f sec\n", time_vec_add(2048, vec_yoru_add));

  printf("\n=== Timing Vec8192 ===\n");
  printf("SIMD:  %f sec\n", time_vec_add(8192, vec_simd_add));
  printf("Naive: %f sec\n", time_vec_add(8192, vec_naive_add));
  printf("Yoru:  %f sec\n", time_vec_add(8192, vec_yoru_add));

  return 0;
}
