#pragma once

#include "rule.hpp"
#include <glm/ext/vector_float3.hpp>
#include <vector>

struct LSystemConfig {
    std::vector<ParaInstruction*> axiomIns;
    std::vector<Rule> rules;
    int maxDepth;
    glm::vec3 tropismDir;
    float tropismSusceptibility;
};