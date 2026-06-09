#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "rule.hpp"
#include "tokens.hpp"

inline std::vector<char> operators = {'*', '+', '-', '/', '^'};

ParaInstruction* encodeInstruction(std::string instructionToEncode, std::unordered_map<std::string, float> constants, const std::vector<char> operators);
std::vector<ParaInstruction*> stringToInsVec(std::string toEncode, std::unordered_map<std::string, float> constants);
