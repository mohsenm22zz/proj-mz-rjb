#pragma once

#include <vector>
#include <string>
#include "Circuit.h"

using namespace std;

bool command_handling(Circuit &circuit, const vector<string> &cmds, vector<vector<string>> &analysisCommands);
void handleErrors(const Circuit &circuit);
Circuit readCircuitFromFile(const string &filename);
void saveResultsToFile(const Circuit &circuit, const string &filename);