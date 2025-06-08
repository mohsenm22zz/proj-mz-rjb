#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "LinearSolver.h"

using namespace std;

vector<double> gaussianElimination(vector<vector<double>> A, vector<double> x) {
    int n = A.size();
    /// مقادیر اولیه
    vector<vector<double>> A_i = A;
    vector<double> x_i = x;
    /*
    [A][v]=[x]
    [A|x] -> مثلث پایین ۰
    [A_new][v]=[x_new]
     حل از پایین
    */
    /// max kardan ghotr asli (صفر نباید باشند)
    for (int i = 0; i < n; i++) {
        int r = i;
        for (int k = i + 1; k < n; k++) {
            if (fabs(A_i[k][i]) > fabs(A_i[r][i])) {
                r = k;
            }
        }
        if (r != i) {
            swap(A_i[i], A_i[r]);
            swap(x_i[i], x_i[r]);/// مستقل نوشته می شود
            //display_vec2D(A_i);
        }
        for (int k = i + 1; k < n; k++) {
            double f = A_i[k][i] / A_i[i][i];
            for (int j = i; j < n; j++) {
                A_i[k][j] -= f * A_i[i][j];
            }
            x_i[k] -= f * x_i[i];
            //display_vec2D(A_i);
        }
    }
    vector<double> v(n);
    /// حل از آخر به اول
    for (int i = n - 1; i >= 0; i--) {
        v[i] = x_i[i];
        for (int j = i + 1; j < n; j++) {
            v[i] -= A_i[i][j] * v[j];
        }
        v[i] /= A_i[i][i];
    }
    return v;
}

void test_solver() {
    vector<vector<double>> a = {{1, 6, 3, 6},
                                {2, 3, 5, 6},
                                {4, 8, 1, 3},
                                {8, 3, 5, 7}};
    vector<double> b = {2, 7, 3, 2};
    display_vec2D(a);
    display_vec(gaussianElimination(a, b));
    display_vec(b);
}

void display_vec2D(vector<vector<double>> a) {
    for (auto it = a.begin(); it != a.end(); it++) {
        for (auto itr = it->begin(); itr != it->end(); itr++) {
            cout << *itr << " ";
        }
        cout << endl;
    }
    cout << endl << endl;
}

void display_vec(vector<double> a) {
    for (auto it = a.begin(); it != a.end(); it++) {
        cout << *it << endl;
    }
    cout << endl;
}