# How to run the benchmarks
Since we are trying to utilize the SIMD capabilities, we need to use the
`-mavx` flag. It is also nice to enable `-O3` and `-ffast-math`. The compilation cmd should look like that:
```bash
gcc <benchmark.c> -o <benchmark> -mavx -O3 -ffast-math
```
