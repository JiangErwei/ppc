//
// Created by suyi on 24-5-14.
//
/**
* https://ppc.cs.aalto.fi/ch2/
* The shortcut problem
* 在 v1 的基础上，尝试实现指令级并行 ILP：
* 1. step_trans_vec 中将每次中间量存储到 w 中，为编译器并行化指令提供了可能
*    （考虑一种理想情况，假设 cpu 恰好有 n 个 core，此时每个 core 会开一个 thread 去计算 w[m](0<=m<n)，只需在最后计算 min(w) 即可，性能将大约可提升 n 倍）
* 2. step_trans_para 中则不同，设定将 k 循环中的 n 次 loop 分成 p 块，每一块的结果存储在 w 中，也可以让编译器进行指令的并行化
*    （为什么要分成 p 块？正是由于 cpu cores 通常达不到 n 这个数量级，此时每个 core 会开一个 thread 去计算 w[m](0<=m<p)，
*    与 step_trans_vec 不同在于每个 thread 会执行大约 n/p 次的 w[m] 计算，性能将大约可提升 p 倍）
* 3. 值得一提的是，若将 p 设置成 n，则上述两个算法等效。
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
	delete []t;
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
	delete []t;
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
	delete []t;
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
	delete []t;
}
