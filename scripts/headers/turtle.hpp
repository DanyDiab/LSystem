#pragma once

#include <vector>
#include <stack>
#include <string>
#include "tokens.hpp"

struct Turtle {
    float x;
    float y;
    float theta;
};

extern std::vector<float> points;

void moveTurtleForward(Turtle *turtle);
void recordTurtlePosition(Turtle turtle);
void executeInstruction(Token token);
void executeInstructions();
void readInJSON();
