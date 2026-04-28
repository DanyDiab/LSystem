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

void drawLine(int x, int y, float length, int theta){
    int endX = cos((90 - theta) * DEGTORAD) * length;
    int endY = sin((90 - theta) * DEGTORAD) * length;
    glLineWidth(10);
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_LINES);
        glVertex2f(x, y);
        glVertex2f(endX,  endY);
    glEnd();

    glFlush(); 
}

void update(){
    glutSwapBuffers();
    glutPostRedisplay();
    drawLine(0,0,10,0);
}





std::vector<Token> readInJSON(){
    std::ifstream file(filePath);
    std::string line;


    std::vector<Token> instructions;
    if(!file.is_open()){
        cout << "could not read in instructions";
        return instructions;
    }

    json parsedData;
    file >> parsedData;

    instructions = parsedData["instructions"].get<std::vector<Token>>();
    std::cout << instructions.size() << "\n";
    std::cout << static_cast<int>(instructions[0]) << "\n";
    return instructions;
    
}

int main(int argc, char** argv){
    cout << "init" << "\n";

    readInJSON();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(600, 600);
    glutCreateWindow("OpenGL");
    glutDisplayFunc(update);

    glutMainLoop();

    return 0;
}