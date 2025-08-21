#pragma once

#include <vector>
#include <string>
#include "Circuit.h"
#include "export.h" // Include the export header

// Add the export macro to the functions you want to call from C#
CIRCUITSIMULATOR_API bool command_handling(Circuit &circuit, const std::vector<std::string> &cmds, std::vector<std::vector<std::string>> &analysisCommands);
CIRCUITSIMULATOR_API bool handleErrors(const Circuit &circuit);
CIRCUITSIMULATOR_API Circuit readCircuitFromFile(const std::string &filename);
CIRCUITSIMULATOR_API void saveResultsToFile(const Circuit &circuit, const std::string &filename);
