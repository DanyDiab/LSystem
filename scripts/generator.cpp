#include <iostream>
#include <string>
#include <fstream>
#include <utility>
#include "./headers/imports/json.hpp"
#include "./headers/tokens.hpp"
#include "./headers/rules.hpp"

using namespace std;
using json = nlohmann::json;


string generateExpansion(string axiom, std::map<std::string, std::string>){
    return "test";
}

// <axiom, rules>
std::tuple<string, std::map<std::string, std::string>, int> parseJSON(){
    std::ifstream file("./system.json");
    std::string line;

    std::string axiom;
    std::map<std::string, std::string> rules;
    int theta = 0;

    if (!file.is_open()) return std::make_tuple(axiom, rules, theta);;
    json parsedData;
    file >> parsedData;

    axiom = parsedData["axiom"].get<std::string>();
    rules = parsedData["rules"].get<std::map<std::string, std::string>>();
    theta = parsedData["theta"].get<int>();

    file.close();

    cout << "Axoim:" << axiom << "\n";
    std::cout << "Rules:\n";
    
    for (const auto& [key, value] : rules) {
        std::cout << "  " << key << " ->    " << value << "\n";
    }

    cout << "Theta:" << theta << "\n";


    return std::make_tuple(axiom, rules, theta);
}


int main(int argc, char** argv){
    parseJSON();
    return 0;
}


// read in JSON
// convert to a token Axoim and an array of Rules
// recursivly match the current token to the LHS of rules, when matching, swap it with RHS
// continue until a certain depth is reached