//
// Created by suyi on 24-5-14.
//

#ifndef MATRIX_H
#define MATRIX_H

#pragma once
#include <iomanip>
#include <iostream>
#include <random>

class Matrix {
	// 设置为方阵
private:
	size_t n = 0;
	float *pData = nullptr;

public:
	explicit Matrix(const size_t n, const float val = 0.f, const bool rand = false): n(n) {
		pData = new float[n * n * sizeof(float)];
		if (val != 0.f) {
			for (size_t i = 0; i < n; ++i) {
				for (size_t j = 0; j < n; ++j) {
					if (i == j) {
						pData[i * n + j] = 0.f; // 对角线元素设为 0
					} else {
						pData[i * n + j] = val;
					}
				}
			}
		}
		if (rand) {
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<> dis(1.0, 20.0);

			for (size_t i = 0; i < n; ++i) {
				for (size_t j = 0; j < n; ++j) {
					if (i == j) {
						pData[i * n + j] = 0.f; // 对角线元素设为 0
					} else {
						float random_value = dis(gen);
						float rounded_value = std::round(random_value * 100.0) / 100.0;
						pData[i * n + j] = rounded_value;
					}
				}
			}
		}
	}

	void print() const {
		for (int i = 0; i < n; ++i) {
			for (int j = 0; j < n; ++j) {
				std::cout << std::fixed << std::setw(6) << std::setprecision(2) << pData[i * n + j] << " ";
			}
			std::cout << "\n";
		}
	}

	~Matrix() {
		delete []pData;
	}

	float *get_pdata() const {
		return pData;
	}
};

#endif //MATRIX_H
