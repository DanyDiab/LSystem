#include <iostream>
#include <vector>
#include <fstream>
#include <stack>
#include <nlohmann/json.hpp>
#include <GL/glut.h>
#include <freeglut/freeglut.h>
#include "./headers/tokens.hpp"

using namespace std;
using json = nlohmann::json;

std::string filePath = "./instructions.json";

const int length = 1;
const float DEGTORAD = 0.01745329f;
int thetaDelta = 0;


int windowWidth = 1000;
int windowHeight = 1000;

float lineWidth = 1;

std::vector<Token> instructions;
struct Turtle {
    float x;
    float y;
    float theta;
};

Turtle turtle;
Turtle nextTurtle;
std::stack<Turtle> turtleStack;

void initCamera(int width, int height, float camX, float camY, float projectionScale) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, width, height);

    float halfWidth = ((float)width * projectionScale) / 2.0f;
    float halfHeight = ((float)height * projectionScale) / 2.0f;
    glOrtho(-halfWidth, halfWidth, -halfHeight, halfHeight, 1.0f, -1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(-camX, -camY, 0.0f);
}

void init(){
    turtle = {0, 0, 0};
    nextTurtle = {0, 0, 0};
    initCamera(windowWidth,windowHeight,0 ,200, 2.0f);
}

void moveTurtleForward(Turtle *turtle){
    int theta = turtle->theta;
    float offsetX = cos((90 - theta) * DEGTORAD) * length;
    float offsetY = sin((90 - theta) * DEGTORAD) * length;

    float endX = offsetX + turtle->x;
    float endY = offsetY + turtle->y;

    turtle->x = endX;
    turtle->y = endY;
}


void drawLineBetweenTurtles(Turtle turtle1, Turtle turtle2){
    float startX = turtle1.x;
    float startY = turtle1.y;

    float endX = turtle2.x;
    float endY = turtle2.y;

    glLineWidth(lineWidth);
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_LINES);
        glVertex2f(startX, startY);
        glVertex2f(endX, endY);
    glEnd();

    glFlush(); 
}


void executeInstruction(Token token){
    switch (token) {
        case Token::F: {
            moveTurtleForward(&nextTurtle);
            drawLineBetweenTurtles(turtle,nextTurtle);
            break;
        }
        case Token::G: {
            moveTurtleForward(&nextTurtle);
            drawLineBetweenTurtles(turtle,nextTurtle);
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


void update(){
    glutSwapBuffers();
    executeInstructions();
    glutPostRedisplay();
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

int main(int argc, char** argv){
    readInJSON();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("OpenGL");
    init();
    glutDisplayFunc(update);

    glutMainLoop();

    return 0;
}