#pragma once

#include <string>
#include <vector>
#include <tuple>

#include "rule.hpp"
#include "tokens.hpp"

std::tuple<std::vector<ParaInstruction*>, std::vector<Rule>, int> parseJSON(const string inFile);
int writeInstructionsToJSON(std::vector<ParaInstruction*> expanded, const std::string outFile);
