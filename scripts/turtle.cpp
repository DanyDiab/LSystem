#include "./headers/turtle.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

std::string filePath = "./instructions.json";
const float length = 4;
const float DEGTORAD = 0.01745329f;
int thetaDelta = 0;
std::vector<Token> instructions;
Turtle turtle;
Turtle nextTurtle;
std::stack<Turtle> turtleStack;
std::vector<float> points;
float lineWidth = 1;

void moveTurtleForward(Turtle *turtle){
    glm::vec3 localForward = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 worldForward = turtle->quaternion * localForward;
    turtle->pos += worldForward * length;
}

void rotateTurtle(Turtle *turtle, glm::vec3 axis, float angle){
    float angleRadians = angle * DEGTORAD;
    glm::quat rotation = glm::angleAxis(angleRadians, axis);
    turtle->quaternion = turtle->quaternion * rotation;
}


void recordTurtlePosition(Turtle *turtle){
    points.push_back(turtle->pos.x);
    points.push_back(turtle->pos.y);
    points.push_back(turtle->pos.z);
}

void executeInstruction(Token token){
    switch (token) {
        case Token::F: {
            recordTurtlePosition(&nextTurtle);
            moveTurtleForward(&nextTurtle);
            recordTurtlePosition(&nextTurtle);
            break;
        }
        case Token::G: {
            recordTurtlePosition(&nextTurtle);
            moveTurtleForward(&nextTurtle);
            recordTurtlePosition(&nextTurtle);
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
            rotateTurtle(&nextTurtle, glm::vec3(0,1,0), thetaDelta);
            break;
        }
        case Token::TurnRight: {
            rotateTurtle(&nextTurtle, glm::vec3(0,1,0), -thetaDelta);
            break;
        }
        case Token::TurnAround: {
            rotateTurtle(&nextTurtle, glm::vec3(0,1,0), 180);
            break;
        }
        case Token::PitchDown: {
            rotateTurtle(&nextTurtle, glm::vec3(1,0,0), thetaDelta);
            break;
        }
        case Token::PitchUp: {
            rotateTurtle(&nextTurtle, glm::vec3(1,0,0), -thetaDelta);
            break;
        }
        case Token::RollLeft: {
            rotateTurtle(&nextTurtle, glm::vec3(0,0,1), thetaDelta);
            break;
        }
        case Token::RollRight: {
            rotateTurtle(&nextTurtle, glm::vec3(0,0,1), thetaDelta);
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

std::vector<float> executeInstructions(){
    points.clear();
    points.reserve(instructions.size() * 6);
    turtle.pos = glm::vec3(0, 0, 0);
    turtle.quaternion = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    nextTurtle = turtle;

    for(const auto& instruction : instructions){
        executeInstruction(instruction);
        turtle = nextTurtle;
    }
    
    return points;
}


// remove std::vector<float> from edtern
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