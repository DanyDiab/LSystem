#pragma once

#include <memory>
#include <string>
#include <vector>
#include <tuple>

#include "Config.hpp"
#include "rule.hpp"
#include "tokens.hpp"

std::shared_ptr<LSystemConfig>parseJSON(const string inFile);
int writeInstructionsToJSON(std::vector<ParaInstruction*> expanded, shared_ptr<LSystemConfig> config, const string outFile);
