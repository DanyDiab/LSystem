#include <iostream>
#include <freeglut/freeglut.h>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include "./headers/rules.hpp"
#include "./headers/tokens.hpp"


using namespace std;
using json = nlohmann::json;

std::string filePath = "./instructions.json";

void update(){
    glutSwapBuffers();
    glutPostRedisplay();
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