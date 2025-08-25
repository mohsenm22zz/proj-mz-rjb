#pragma once

#include <vector>

using namespace std;

void display_vec2D(vector<vector<double>> a);

void display_vec(vector<double> a);

void test_solver();

vector<complex<double>> gaussianElimination(vector<vector<complex<double>>> A, vector<complex<double>> b);
