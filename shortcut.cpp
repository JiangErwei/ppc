//
// Created by suyi on 24-5-13.
//
/**
* https://ppc.cs.aalto.fi/ch2/
* The shortcut problem
*/


#include <algorithm>
#include <cstring>
#include <random>
#include <iostream>
#include <iomanip>
#include <chrono>

void create(float *d, size_t n);

void print(const float *mat, const size_t n);

void step(float *r, const size_t n);

void step(float *r, const float *d, int n);

int main() {
	constexpr int n = 2000;
	// const float d[n * n] = {
	// 	0, 8, 2,
	// 	1, 0, 9,
	// 	4, 5, 0,
	// };

	// std::cout << sizeof(float);
	auto *d = new float[n * n * 4];
	create(d, n);
	// print(d, n);

	std::cout << "-------------------------------------------------------------------------------------" << std::endl;
	// float r[n * n];

	auto start = std::chrono::high_resolution_clock::now();
	// step(r, d, n);
	step(d, n);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	// print(r, n);

	std::cout << "Elapsed time: " << elapsed.count() << " s\n";
	delete []d;
}

void create(float *d, const size_t n) {
	// 创建随机数生成器
	std::random_device rd; // 用于生成种子
	std::mt19937 gen(rd()); // 随机数引擎使用 Mersenne Twister 算法

	// 创建均匀分布 [1.0, 20.0)
	std::uniform_real_distribution<> dis(1.0, 20.0);

	// 生成随机浮点数并保留两位小数
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


void print(const float *mat, const size_t n) {
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			std::cout << std::fixed << std::setw(6) << std::setprecision(2) << mat[i * n + j] << " ";
		}
		std::cout << "\n";
	}
}

// floyd algo
void step(float *r, const float *d, int n) {
	memcpy(r, d, n * n * sizeof(float));
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < n; ++j)
			for (size_t k = 0; k < n; ++k)
				r[i * n + j] = std::min(r[i * n + j], r[i * n + k] + r[k * n + j]);
}


void step(float *r, const size_t n) {
	// memcpy(r, d, n * n * sizeof(float));
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < n; ++j)
			for (size_t k = 0; k < n; ++k)
				r[i * n + j] = std::min(r[i * n + j], r[i * n + k] + r[k * n + j]);
}

