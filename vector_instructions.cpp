//
// Created by suyi on 24-5-16.
//

#include <iostream>

typedef float float8_t __attribute__ ((vector_size (8 * sizeof(float))));


int main() {
	float8_t a[] {1, 2, 3, 4, 5, 6, 7, 28};
	float8_t b[] {11, 12, 13, 14, 15, 16, 17, 8};
	float8_t c = *a + *b;


	for (int i = 0; i < 8; ++i) {
		std::cout << c[i] << ' ';
	}
	std::cout << std::endl;

	return 0;
}

