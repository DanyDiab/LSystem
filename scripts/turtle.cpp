#include "./headers/turtle.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

std::string filePath = "./instructions.json";
const int length = 4;
const float DEGTORAD = 0.01745329f;
int thetaDelta = 0;
std::vector<Token> instructions;
Turtle turtle;
Turtle nextTurtle;
std::stack<Turtle> turtleStack;
std::vector<float> points;
float lineWidth = 1;

void moveTurtleForward(Turtle *turtle){
    int theta = turtle->theta;
    float offsetX = cos((90 - theta) * DEGTORAD) * length;
    float offsetY = sin((90 - theta) * DEGTORAD) * length;

    float endX = offsetX + turtle->x;
    float endY = offsetY + turtle->y;

    turtle->x = endX;
    turtle->y = endY;
}

void recordTurtlePosition(Turtle turtle){
    points.push_back(turtle.x);
    points.push_back(turtle.y);
}

void executeInstruction(Token token){
    switch (token) {
        case Token::F: {
            recordTurtlePosition(nextTurtle);
            moveTurtleForward(&nextTurtle);
            recordTurtlePosition(nextTurtle);
            break;
        }
        case Token::G: {
            recordTurtlePosition(nextTurtle);
            moveTurtleForward(&nextTurtle);
            recordTurtlePosition(nextTurtle);
            break;
        }
        case Token::f: {
            moveTurtleForward(&nextTurtle);
            break;
        }
        case Token::g: {
            moveTurtleForward(&nextTurtle);
            break;
        }
        case Token::Z: {
            break;
        }
        case Token::TurnLeft: {
            nextTurtle.theta -= thetaDelta;
            break;
        }
        case Token::TurnRight: {

            nextTurtle.theta += thetaDelta;
            break;
        }
        case Token::TurnAround: {
            nextTurtle.theta += 180;
            break;
        }
        case Token::PitchDown: {
            break;
        }
        case Token::PitchUp: {
            break;
        }
        case Token::RollLeft: {
            break;
        }
        case Token::RollRight: {
            break;
        }
        case Token::PushState: {
            turtleStack.push(nextTurtle);
            break;
        }
        case Token::PopState: {
            nextTurtle = turtleStack.top();
            turtleStack.pop();
            break;
        }
        case Token::DecreaseWidth: {
            if (lineWidth <= 1.0f) {
                return;
            }
            lineWidth -= 1.0f;
            break;
        }
        case Token::IncreaseWidth: {
            lineWidth += 1.0f;
            break;
        }
        case Token::NextColor: {
            break;
        }
        case Token::DrawPolygon: {
            break;
        }
        case Token::NonTerminal:{
            break;
        }
        default: {
            cout << "defaulting" << "\n";

            break;
        }
    }
}

void executeInstructions(){
    for(const auto& instruction : instructions){
        executeInstruction(instruction);
        turtle = nextTurtle;
    }

    turtle = {0, 0, 0};
    nextTurtle = turtle;
}

void readInJSON(){
    std::ifstream file(filePath);

    if(!file.is_open()){
        cout << "could not read in instructions";
    }

    json parsedData;
    file >> parsedData;

    instructions = parsedData["instructions"].get<std::vector<Token>>();
    thetaDelta = parsedData["theta"].get<int>();
}
