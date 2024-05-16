//
// Created by suyi on 24-5-14.
//
/**
* https://ppc.cs.aalto.fi/ch2/
* The shortcut problem
* 在 v0 的基础上增加内存访问优化—— step_trans 为避免对内存的非顺序读取，采用矩阵转置预处理
*/

#include <chrono>

#include "matrix.h"

void step(float *r, const float *d, const size_t n);
void step_trans(float *r, const float *d, const size_t n);

int main() {
	constexpr int n = 4000;

	Matrix d(n, 0.f, true);
	// d.print();

	Matrix r(n);
	// r.print();

	auto start = std::chrono::high_resolution_clock::now();
	step(r.get_pdata(), d.get_pdata(), n);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n";

	start = std::chrono::high_resolution_clock::now();
	step_trans(r.get_pdata(), d.get_pdata(), n);
	end = std::chrono::high_resolution_clock::now();
	elapsed = end - start;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n";
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


void step_trans(float *r, const float *d, const size_t n) {
	auto *t = new float[n * n * sizeof(float)];

	for (size_t i = 0; i < n; ++i) {
		for (size_t j = 0; j < n; ++j) {
			t[i * n + j] = d[j * n + i];
		}
	}

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
	delete []t;
}
