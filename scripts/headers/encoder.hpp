#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "rule.hpp"
#include "tokens.hpp"

ParaInstruction* encodeInstruction(std::string instructionToEncode, std::unordered_map<std::string, float> constants, const std::vector<char> operators = {'*', '+', '-', '/', '^'});
