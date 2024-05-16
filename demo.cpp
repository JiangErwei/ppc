#include <iostream>
#include <vector>
#include <limits>

void step(std::vector<float>& r, const std::vector<float>& d, int n);
int main() {
	constexpr int n = 3;
	std::vector<float> d = {
		0, 8, 2,
		1, 0, 9,
		4, 5, 0,
	};
	std::vector<float> r(n * n);
	step(r, d, n);
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			std::cout << r[i*n + j] << " ";
		}
		std::cout << "\n";
	}
}

void step(std::vector<float>& r, const std::vector<float>& d, int n) {
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			float v = std::numeric_limits<float>::infinity();
			for (int k = 0; k < n; ++k) {
				float x = d[n*i + k];
				float y = d[n*k + j];
				float z = x + y;
				v = std::min(v, z);
			}
			r[n*i + j] = v;
		}
	}
}
