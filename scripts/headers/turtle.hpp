#pragma once

#include <vector>
#include <stack>
#include <string>
#include <glm/glm.hpp>
#include "tokens.hpp"
#include <glm/gtc/quaternion.hpp>
#include "./rule.hpp"


struct Turtle {
    glm::vec3 pos;
    glm::quat quaternion; 
    float scale;
};

void moveTurtleForward(Turtle *turtle, float distance);
void recordTurtlePosition(Turtle *turtle, float distance);
void executeInstruction(const ParaInstructionTok* instruction);
std::tuple<std::vector<glm::mat4>, std::vector<float>> executeInstructions();
std::vector<ParaInstructionTok> readInJSON(const std::string& filePath);
