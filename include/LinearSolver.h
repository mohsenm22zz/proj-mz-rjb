#pragma once

#include <vector>

using namespace std;

void display_vec2D(vector<vector<double>> a);

void display_vec(vector<double> a);

void test_solver();

vector<double> gaussianElimination(vector<vector<double>> A, vector<double> b);