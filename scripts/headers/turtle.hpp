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

extern std::vector<float> points;

void moveTurtleForward(Turtle *turtle);
void recordTurtlePosition(Turtle turtle);
void executeInstruction(Token token);
void executeInstructions();
void readInJSON();
