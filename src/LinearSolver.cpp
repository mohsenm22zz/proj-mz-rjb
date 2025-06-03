#include "LinearSolver.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<double> gaussianElimination(vector<vector<double>> A, vector<double> b_in) {
    vector<double> b = b_in;
    int n = A.size();
    if (n == 0) return {};
    if (A[0].size() != (size_t)n || b.size() != (size_t)n) {
        cerr << "Error: Matrix dimensions mismatch in Gaussian elimination." << endl;
        return {};
    }

    for (int i = 0; i < n; ++i) {
        int maxR = i;
        for (int k = i + 1; k < n; ++k) {
            if (fabs(A[k][i]) > fabs(A[maxR][i])) {
                maxR = k;
            }
        }
        swap(A[i], A[maxR]);
        swap(b[i], b[maxR]);

        if (fabs(A[i][i]) < 1e-9) {
            cerr << "Warning: Matrix is singular or nearly singular during Gaussian elimination (pivot is near zero at row " << i << ")." << endl;
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

    vector<double> x(n);
    for (int i = n - 1; i >= 0; --i) {
        if (fabs(A[i][i]) < 1e-9) {
            cerr << "Warning: Matrix is singular or nearly singular during back substitution (A[i][i] is near zero at row " << i << ")." << endl;
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