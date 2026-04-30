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


void rotateTurtle(Turtle *turtle, glm::vec3 rotation){
    glm::quat quaterion = turtle->quaternion;

    glm::quat rotateQuat = glm::quat(rotation);
    turtle->quaternion = quaterion * rotateQuat;
}


void recordTurtlePosition(Turtle *turtle){
    points.push_back(turtle->pos.x);
    points.push_back(turtle->pos.y);
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
            rotateTurtle(&nextTurtle, glm::vec3(0,0,-thetaDelta));
            break;
        }
        case Token::TurnRight: {
            rotateTurtle(&nextTurtle, glm::vec3(0,0,thetaDelta));
            break;
        }
        case Token::TurnAround: {
            rotateTurtle(&nextTurtle, glm::vec3(0,0,180));
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
    turtle.pos = glm::vec3(0, 0, 0);
    turtle.quaternion = glm::quat();
    nextTurtle = turtle;

    for(const auto& instruction : instructions){
        executeInstruction(instruction);
        turtle = nextTurtle;
    }



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
