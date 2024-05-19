//
// Created by suyi on 24-5-14.
//
/**
 * https://ppc.cs.aalto.fi/ch2/v3/
 * The shortcut problem
 * 在 v3 和 vector_instructions.cpp 的基础上：增加对 SIMD 的支持，同时考虑了内存对齐问题（memory_alignment.cpp）
 *
 */

#include <algorithm>
#include <chrono>

#include "matrix.h"

typedef float float8_t __attribute__ ((vector_size(8 * sizeof(float))));

constexpr float inf = std::numeric_limits<float>::infinity();

//constexpr float8_t f8inf{
//    inf, inf, inf, inf, inf, inf, inf, inf
//};

void step_trans(float *r, const float *d, size_t n); // 保留用于对比测试

void step_trans_ilp_omp(float *r, const float *d, size_t n);


// 计时函数
inline void measure_time(const std::string &func_name, const std::function<void()> &func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << func_name << " elapsed time: " << elapsed.count() << " s\n";
}


int main() {
    constexpr int n = 2400;

    Matrix d(n, 0.f, true);
    // d.print();

    Matrix r(n);
    // r.print();

    measure_time("step_trans", [&]() {
        step_trans(r.get_pdata(), d.get_pdata(), n);
    });
    // r.print();

    measure_time("step_trans_ilp_omp", [&]() {
        step_trans_ilp_omp(r.get_pdata(), d.get_pdata(), n);
    });
    // r.print();
}

void step_trans(float *r, const float *d, const size_t n) {
    auto *t = new float[n * n * sizeof(float)];
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            t[i * n + j] = d[j * n + i];
        }
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            float v = inf;
            for (int k = 0; k < n; ++k) {
                float x = d[n * i + k];
                float y = t[n * j + k];
                float z = x + y;
                v = std::min(v, z);
            }
            r[n * i + j] = v;
        }
    }
    delete[] t;
}

void step_trans_ilp_omp(float *r, const float *d, const size_t n) {
    auto *t = new float[n * n * sizeof(float)];
#pragma omp parallel for
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            t[i * n + j] = d[j * n + i];
        }
    }
    constexpr size_t p = 4;
#pragma omp parallel for
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            float w[p];
            for (float &item: w)
                item = inf;
            for (int k = 0; k < n / p; ++k) {
                for (int m = 0; m < p; ++m) {
                    float x = d[n * i + k * p + m];
                    float y = t[n * j + k * p + m];
                    float z = x + y;
                    w[m] = std::min(w[m], z);
                }
            }
            r[n * i + j] = *std::min_element(w, w + p);
        }
    }
    delete[] t;
}

/* 需要注意：
 * 在 data -> vectors 时，由于每个 vector 长度为 8，而 data_size 为 n*n，
 * 1. if (n*n)%8 == 0: 则 data 中的全部元素都能均匀转入 vector 中
 * 2. if (n*n)%8 != 0: 则需要考虑 padded，最终效果要求为每个 vector = [x_1, x_2 ... x_m, inf, ... inf]
 */
void step_trans_simd_omp(float *r, const float *d, const size_t n) {
    // padded, converted to vectors
    constexpr size_t vec_len = 8;
    size_t blocks = (n + vec_len - 1) / vec_len;

    std::vector<float8_t> vd(n * blocks);
    std::vector<float8_t> vt(n * blocks);

    for (size_t i = 0; i < n; ++i) {

    }

}