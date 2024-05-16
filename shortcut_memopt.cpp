//
// Created by suyi on 24-5-13.
//

/**
* https://ppc.cs.aalto.fi/ch2/
* The shortcut problem
* 从内存访问层面进行优化
*/

#include <omp.h>
#include <algorithm>
#include <cstring>
#include <random>
#include <iostream>
#include <iomanip>
#include <chrono>

void create(float *d, size_t n);

void trans(float *, const float *, const size_t);

void step(float *, const float *, const size_t);

void step(float *, const float *, const float *, const size_t);

int main() {
	constexpr int n = 4000;

	auto *d = new float[n * n * 4];
	create(d, n);

	auto *t = new float[n * n * 4];
	trans(t, d, n);

	// float r[n * n];
	auto *r = new float[n * n * 4];


	auto start = std::chrono::high_resolution_clock::now();
	// step(r, d, n);
	step(r, d, t, n);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n";

	start = std::chrono::high_resolution_clock::now();
	step(r, d, n);
	// step(r, d, t, n);
	end = std::chrono::high_resolution_clock::now();
	elapsed = end - start;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n";

	delete []d;
	delete []t;
	delete []r;
}


void create(float *d, const size_t n) {
	std::random_device rd; // 用于生成种子
	std::mt19937 gen(rd()); // 随机数引擎使用 Mersenne Twister 算法
	std::uniform_real_distribution<> dis(1.0, 20.0);

	for (size_t i = 0; i < n; ++i) {
		for (size_t j = 0; j < n; ++j) {
			if (i == j) {
				d[i * n + j] = 0.0f; // 对角线元素设为 0
			} else {
				float random_value = dis(gen);
				// 保留两位小数
				float rounded_value = std::round(random_value * 100.0) / 100.0;
				d[i * n + j] = rounded_value;
			}
		}
	}
}

void trans(float *t, const float *d, const size_t n) {
	for (size_t i = 0; i < n; ++i) {
		for (size_t j = 0; j < n; ++j) {
			t[i * n + j] = d[j * n + i];
		}
	}
}

void step(float *r, const float *d, const size_t n) {
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			float v = std::numeric_limits<float>::infinity();
			for (int k = 0; k < n; ++k) {
				float x = d[n * i + k];
				float y = d[n * k + j];
				float z = x + y;
				v = std::min(v, z);
			}
			r[n * i + j] = v;
		}
	}
}

void step(float *r, const float *d, const float *t, const size_t n) {
#pragma omp parallel for
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			float v = std::numeric_limits<float>::infinity();
			for (int k = 0; k < n; ++k) {
				float x = d[n * i + k];
				float y = t[n * j + k];
				float z = x + y;
				v = std::min(v, z);
			}
			r[n * i + j] = v;
		}
	}
}
