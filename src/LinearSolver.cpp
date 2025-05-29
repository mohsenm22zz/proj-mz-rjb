#include "LinearSolver.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

std::vector<double> gaussianElimination(std::vector<std::vector<double>> A, std::vector<double> b_in) {
    std::vector<double> b = b_in;
    int n = A.size();
    if (n == 0) return {};
    if (A[0].size() != (size_t)n || b.size() != (size_t)n) {
        std::cerr << "Error: Matrix dimensions mismatch in Gaussian elimination." << std::endl;
        return {};
    }

    for (int i = 0; i < n; ++i) {
        int maxR = i;
        for (int k = i + 1; k < n; ++k) {
            if (std::fabs(A[k][i]) > std::fabs(A[maxR][i])) {
                maxR = k;
            }
        }
        std::swap(A[i], A[maxR]);
        std::swap(b[i], b[maxR]);

        if (std::fabs(A[i][i]) < 1e-9) {
            std::cerr << "Warning: Matrix is singular or nearly singular during Gaussian elimination (pivot is near zero at row " << i << ")." << std::endl;
            return {};
        }

        // Eliminate forward
        for (int k = i + 1; k < n; ++k) {
            double factor = A[k][i] / A[i][i];
            for (int j = i; j < n; ++j) {
                A[k][j] -= factor * A[i][j];
            }
            b[k] -= factor * b[i];
        }
    }

    std::vector<double> x(n);
    for (int i = n - 1; i >= 0; --i) {
        if (std::fabs(A[i][i]) < 1e-9) {
            std::cerr << "Warning: Matrix is singular or nearly singular during back substitution (A[i][i] is near zero at row " << i << ")." << std::endl;
            return {};
        }
        x[i] = b[i];
        for (int j = i + 1; j < n; ++j) {
            x[i] -= A[i][j] * x[j];
        }
        x[i] /= A[i][i];
    }
    return x;
}