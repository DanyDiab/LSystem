#pragma once

#include <vector>
#include <stack>
#include <string>
#include <glm/glm.hpp>
#include "tokens.hpp"
#include <glm/gtc/quaternion.hpp>

struct Turtle {
    glm::vec3 pos;
    glm::quat quaternion; 
};

void moveTurtleForward(Turtle *turtle);
void recordTurtlePosition(Turtle turtle);
void executeInstruction(Token token);
std::vector<float> executeInstructions();
void readInJSON();
