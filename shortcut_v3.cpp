//
// Created by suyi on 24-5-14.
//
/**
 * https://ppc.cs.aalto.fi/ch2/
 * The shortcut problem
 * 在 v2 的基础上：增加对 OpenMP 的支持
 * 需要分析数据竞争问题
 * 1. 局部变量（保证 thread 可以复制局部变量）
 * 2. "共享变量"
 */

#include <algorithm>
#include <chrono>

#include "matrix.h"

constexpr float inf = std::numeric_limits<float>::infinity();

void step_trans(float *r, const float *d, const size_t n);

void step_trans_omp(float *r, const float *d, const size_t n);

void step_trans_ilp(float *r, const float *d, const size_t n);

void step_trans_ilp_omp(float *r, const float *d, const size_t n);

int main() {
  constexpr int n = 2400;

  Matrix d(n, 0.f, true);
  // d.print();

  Matrix r(n);
  // r.print();

  auto start = std::chrono::high_resolution_clock::now();
  step_trans(r.get_pdata(), d.get_pdata(), n);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Elapsed time: " << elapsed.count() << " s\n";
  // r.print();

  // start = std::chrono::high_resolution_clock::now();
  // step_trans_omp(r.get_pdata(), d.get_pdata(), n);
  // end = std::chrono::high_resolution_clock::now();
  // elapsed = end - start;
  // std::cout << "Elapsed time: " << elapsed.count() << " s\n";
  // // r.print();

  start = std::chrono::high_resolution_clock::now();
  step_trans_ilp_omp(r.get_pdata(), d.get_pdata(), n);
  end = std::chrono::high_resolution_clock::now();
  elapsed = end - start;
  std::cout << "Elapsed time: " << elapsed.count() << " s\n";
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

void step_trans_omp(float *r, const float *d, const size_t n) {
  auto *t = new float[n * n * sizeof(float)];
#pragma omp parallel for
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      t[i * n + j] = d[j * n + i];
    }
  }
#pragma omp parallel for
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

void step_trans_ilp(float *r, const float *d, const size_t n) {
  auto *t = new float[n * n * sizeof(float)];
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      t[i * n + j] = d[j * n + i];
    }
  }
  constexpr size_t p = 12;
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      float w[p];
      for (float &item : w)
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
      for (float &item : w)
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
