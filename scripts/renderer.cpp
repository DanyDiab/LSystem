#include <iostream>
#include <freeglut/freeglut.h>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include "./headers/rules.hpp"
#include "./headers/tokens.hpp"
#include <GL/glut.h>

using namespace std;
using json = nlohmann::json;

std::string filePath = "./instructions.json";

const int length = 2;
const float DEGTORAD = 0.01745329f;

struct Turtle {
    int x;
    int y;
    int theta;
};

Turtle turtle;
Turtle nextTurtle;

void init(){
    turtle = {0, 0, 0};
    nextTurtle = {0, 0, 0};
}

void moveTurtleForward(Turtle turtle){
    int theta = turtle.theta;
    float offsetX = cos((90 - theta) * DEGTORAD) * length;
    float offsetY = sin((90 - theta) * DEGTORAD) * length;

    float endX = offsetX + turtle.x;
    float endY = offsetY + turtle.y;

    turtle.x = endX;
    turtle.y = endY;
}

void executeInstruction(Token token){
    switch (token) {
        case Token::F: {
            moveTurtleForward(nextTurtle);
            drawLineBetweenTurtles(turtle,nextTurtle);
            break;
        }
        case Token::G: {
            moveTurtleForward(nextTurtle);
            drawLineBetweenTurtles(turtle,nextTurtle);
            break;
        }
        case Token::f: {
            moveTurtleForward(nextTurtle);
            break;
        }
        case Token::Z: {
            break;
        }
        case Token::TurnLeft: {
            break;
        }
        case Token::TurnRight: {
            break;
        }
        case Token::TurnAround: {
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
            break;
        }
        case Token::PopState: {
            break;
        }
        case Token::DecreaseWidth: {
            break;
        }
        case Token::IncreaseWidth: {
            break;
        }
        case Token::NextColor: {
            break;
        }
        case Token::DrawPolygon: {
            break;
        }
        default: {
            break;
        }
    }
}

void drawLineBetweenTurtles(Turtle turtle1, Turtle turtle2){
    int startX = turtle1.x;
    int startY = turtle1.y;

    int endX = turtle2.x;
    int endY = turtle2.y;
    glLineWidth(10);
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_LINES);
        glVertex2f(startX, startY);
        glVertex2f(endX, endY);
    glEnd();

    glFlush(); 
}

void update(){
    glutSwapBuffers();
    glutPostRedisplay();
}

std::vector<Token> readInJSON(){
    std::ifstream file(filePath);

    std::vector<Token> instructions;
    if(!file.is_open()){
        cout << "could not read in instructions";
        return instructions;
    }

    json parsedData;
    file >> parsedData;

    instructions = parsedData["instructions"].get<std::vector<Token>>();
    return instructions;
}

int main(int argc, char** argv){
    readInJSON();
    init();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(600, 600);
    glutCreateWindow("OpenGL");
    glutDisplayFunc(update);

    glutMainLoop();

    

    return 0;
}