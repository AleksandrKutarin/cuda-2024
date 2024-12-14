// Copyright (c) 2024 Vinichuk Timofey
#include "naive_gemm_omp.h"
#include <omp.h>
#include <vector>

std::vector<float> NaiveGemmOMP(
    const std::vector<float>& a,
    const std::vector<float>& b,
    int n) {

    std::vector<float> c(n * n, 0.0f);
    #pragma omp parallel for shared(a, b, c)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n;  j++)
                for (int k = 0; k < n; k++)
                    c[i * n + j] += a[i * n + k] * b[n * k + j];
    return c;
}