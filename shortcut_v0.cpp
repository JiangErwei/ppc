//
// Created by suyi on 24-5-14.
//
/**
* https://ppc.cs.aalto.fi/ch2/
* The shortcut problem
* 采用 Floyd 算法求解多源最短路问题
*/

#include <chrono>

#include "matrix.h"

void step(float *r, const float *d, const size_t n);

int main() {
	constexpr int n = 2500;

	Matrix d(n, 0.f, true);
	// d.print();

	Matrix r(n);
	// r.print();

	auto start = std::chrono::high_resolution_clock::now();
	step(r.get_pdata(), d.get_pdata(), n);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
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

