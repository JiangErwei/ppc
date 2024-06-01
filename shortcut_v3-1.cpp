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


void step_trans(float *r, const float *d, size_t n); // 保留用于对比测试

void step_trans_ilp_omp(float *r, const float *d, size_t n);

void step_trans_simd_omp(float *r, const float *d, size_t n);


// 计时函数
inline void measure_time(const std::string &func_name, const std::function<void()> &func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << func_name << " elapsed time: " << elapsed.count() << " s\n";
}

constexpr float8_t f8inf{
        inf, inf, inf, inf, inf, inf, inf, inf
};

static inline float hmin8(float8_t vv) {
    float v = inf;
    for (int i = 0; i < 8; ++i) {
        v = std::min(vv[i], v);
    }
    return v;
}


int main() {
    constexpr int n = 4000;

    Matrix d(n, 0.f, true);
    // d.print();

    Matrix r(n);
    // r.print();
    // step_trans_simd_omp(r.get_pdata(), d.get_pdata(), n);


//    measure_time("step_trans", [&]() {
//        step_trans(r.get_pdata(), d.get_pdata(), n);
//    });
//    // r.print();

    measure_time("step_trans_ilp_omp", [&]() {
        step_trans_ilp_omp(r.get_pdata(), d.get_pdata(), n);
    });
    // r.print();

    measure_time("step_trans_simd_omp", [&]() {
        step_trans_simd_omp(r.get_pdata(), d.get_pdata(), n);
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

    // d:n*n  vd:n*(blocks*vec_len)
#pragma omp parallel for
    for (size_t i = 0; i < n; ++i) {
        for (size_t b_j = 0; b_j < blocks; ++b_j) {
            for (size_t v_j = 0; v_j < vec_len; ++v_j) {
                int j = b_j * vec_len + v_j;
                vd[i * blocks + b_j][v_j] = j < n ? d[n * i + j] : inf;
                vt[i * blocks + b_j][v_j] = j < n ? d[n * j + i] : inf;
            }
        }
    }
#pragma omp parallel for
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            float8_t vv = f8inf;
            for (size_t k = 0; k < blocks; ++k) {
                float8_t x = vd[blocks * i + k];
                float8_t y = vd[blocks * j + k];
                float8_t z = x + y;
                vv = vv > z ? z : vv;
            }
            r[n * i + j] = hmin8(vv);
        }
    }


}

// 测试是否转置正确
//    for (size_t i = 0; i < n; ++i) {
//        for (size_t b_j = 0; b_j < blocks; ++b_j) {
//            for (size_t v_j = 0; v_j < vec_len; ++v_j) {
////                std::cout << std::fixed << std::setw(6) << std::setprecision(2) << vd[i * blocks + b_j][v_j] << " ";
//                std::cout << std::fixed << std::setw(6) << std::setprecision(2) << vt[i * blocks + b_j][v_j] << " ";
//                // vt[i * blocks + b_j][v_j];
//            }
//        }
//        std::cout << std::endl;
//    }
