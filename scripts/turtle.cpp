#include "./headers/turtle.hpp"
#include "./headers/tokens.hpp"
#include "./headers/rule.hpp"

#include <glm/ext/quaternion_geometric.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <fstream>
#include <cmath>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

const std::string filePath = "./instructions.json";
const float DEGTORAD = 0.01745329f;
int thetaDelta = 0;
Turtle turtle;
Turtle nextTurtle;
std::stack<Turtle> turtleStack;
std::vector<glm::mat4> models;
std::vector<float> widths; 
float lineWidth = 1;

void moveTurtleForward(Turtle *turtle, float distance){
    glm::vec3 localForward = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 worldForward = turtle->quaternion * localForward;
    turtle->pos += worldForward * distance;
}

void rotateTurtle(Turtle *turtle, glm::vec3 axis, float angle){
    float angleRadians = angle * DEGTORAD;
    glm::quat rotation = glm::angleAxis(angleRadians, axis);
    turtle->quaternion = glm::normalize(turtle->quaternion * rotation);
}

void RollTurtleToHorizontal(Turtle *turtle){
    glm::vec3 globalUp = glm::vec3(0,1,0);

    glm::quat tQuat = turtle->quaternion;
    
    glm::vec3 heading = tQuat * glm::vec3(0.0f,0.0f, 1.0f);
    glm::vec3 up = tQuat * glm::vec3(0.0f,1.0f, 0.0f);

    if (std::abs(glm::dot(globalUp, heading)) > 0.999f) return;

    glm::vec3 right = glm::normalize(glm::cross(globalUp, heading));
    glm::vec3 newUp = glm::normalize(glm::cross(heading, right));
    glm::mat3 rotation = glm::mat3(right, newUp, heading);

    glm::quat newQuat = glm::quat_cast(rotation);

    turtle->quaternion = newQuat;
}


void recordTurtlePosition(Turtle *turtle, float distance){
    float scale = turtle->scale;
    glm::mat4 identity = glm::mat4(1.0f);

    glm::mat4 model = glm::translate(identity,turtle->pos);

    model *= glm::mat4_cast(turtle->quaternion);

    model = glm::scale(model, glm::vec3(scale, scale, distance));

    models.push_back(model);
    widths.push_back(scale);
}

void executeInstruction(const ParaInstructionTok* instruction){
    Token token = instruction->token;
    std::vector<float> params = instruction->params;
    
    switch (token) {
        case Token::F: {
            float distance = params[0];

            recordTurtlePosition(&nextTurtle, distance);
            moveTurtleForward(&nextTurtle, distance);

            break;
        }
        case Token::G: {
            float distance = params[0];

            recordTurtlePosition(&nextTurtle, distance);
            moveTurtleForward(&nextTurtle, distance);

            break;
        }
        case Token::f: {
            float distance = params[0];

            moveTurtleForward(&nextTurtle, distance);
            break;
        }
        case Token::g: {
            float distance = params[0];

            moveTurtleForward(&nextTurtle, distance);
            break;
        }
        case Token::Z: {
            break;
        }
        case Token::Yaw: {
            float angle = params.at(0);

            rotateTurtle(&nextTurtle, glm::vec3(0,1,0), angle);
            break;
        }
        case Token::Pitch: {
            float angle = params.at(0);

            rotateTurtle(&nextTurtle, glm::vec3(1,0,0), angle);
            break;
        }
        case Token::Roll: {
            float angle = params.at(0);

            rotateTurtle(&nextTurtle, glm::vec3(0,0,1), angle);
            break;
        }
        case Token::TurnAround: {
            rotateTurtle(&nextTurtle, glm::vec3(0,1,0), 180);
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
        case Token::Width: {
            nextTurtle.scale = params[0] * 0.05f;
            break;
        }
        case Token::HorizontalRollAlign: {
            RollTurtleToHorizontal(&nextTurtle);
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

std::tuple<std::vector<glm::mat4>, std::vector<float>> executeInstructions(){
    std::vector<ParaInstructionTok> instructions = readInJSON(filePath);
    models.clear();
    widths.clear();
    while (!turtleStack.empty()) {
        turtleStack.pop();
    }
    models.reserve(instructions.size());
    widths.reserve(instructions.size());

    turtle.scale = 1.0f;
    turtle.pos = glm::vec3(0, 0, 0);
    turtle.quaternion = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    nextTurtle = turtle;

    for(const auto& instruction : instructions){
        executeInstruction(&instruction);
        turtle = nextTurtle;
    }
    
    return std::make_tuple(models,widths);
}


std::vector<ParaInstructionTok> readInJSON(const std::string& filePath) {
    std::vector<ParaInstructionTok> parsedInstructions;
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << "\n";
        return parsedInstructions;
    }

    nlohmann::json jsonData;
    file >> jsonData;

    if (!jsonData.contains("instructions")) {
        std::cerr << "Error: JSON root does not contain 'instructions' array.\n";
        return parsedInstructions;
    }

    const nlohmann::json& instructionsArray = jsonData["instructions"];

    if (!instructionsArray.is_array()) {
        std::cerr << "Error: 'instructions' key is not an array.\n";
        return parsedInstructions;
    }

    for (const nlohmann::json& item : instructionsArray) {
        if (!item.contains("token") || !item["token"].is_string()) {
            continue;
        }

        ParaInstructionTok instruction;
        std::string tokenString = item["token"].get<std::string>();
        instruction.token = charToToken(tokenString[0]);

        if (!item.contains("params") || !item["params"].is_array()) {
            parsedInstructions.push_back(instruction);
            continue;
        }

        const nlohmann::json& paramsArray = item["params"];
        
        for (const nlohmann::json& param : paramsArray) {
            if (!param.is_number()) {
                continue;
            }
            
            float paramValue = param.get<float>();
            instruction.params.push_back(paramValue);
        }

        parsedInstructions.push_back(instruction);
    }

    return parsedInstructions;
}