#include <iostream>
#include <string>
#include <fstream>
#include <utility>
#include <nlohmann/json.hpp>
#include "./headers/tokens.hpp"
#include <vector>
#include <sstream>

using namespace std;
using json = nlohmann::json;

int MAXDEPTH = 10;

std::string recurExpand(std::string curr, std::map<char, std::string> rules, int numRules, int depth){
    if(depth == MAXDEPTH) return curr;
    std::string nextExpansion;
    
    for(const auto& character : curr){
        if(std::isspace(character)) continue;
        bool foundExpansion = false;
        for(int i = 0 ; i < numRules; i++){
            std::map<char, std::string>::iterator it = rules.find(character);
            if(!(it == rules.end())){
                foundExpansion = true;
                nextExpansion += it->second;
                break;
            }
        }
        if(!foundExpansion){
            nextExpansion += character;
        }
    }

    return recurExpand(nextExpansion,rules,numRules, depth+1);
}

int writeInstructionsToJSON(std::string expanded, float theta){


    std::vector<Token> tokens;

    for(const auto& c : expanded){
        if(std::isspace(c)) continue;
        Token token = charToToken(c);
        tokens.push_back(token);
    }

    json data;
    data["instructions"] = tokens;
    data["theta"] = theta;

    std::ofstream outputFile("./instructions.json");

    if(!outputFile.is_open()) return 1;

    outputFile << data.dump(4);
    outputFile.close();
    return 0;
}

std::string generateExpansion(std::tuple<string, std::map<char, std::string>, float> data){
    std::string axiom = std::get<0>(data);
    std::map<char, std::string> rules = std::get<1>(data);
    int numRules = rules.size();
    return recurExpand(axiom,rules,numRules,0);
}

// <axiom, rules>
std::tuple<string, std::map<char, std::string>, float> parseJSON(){
    std::ifstream file("./system.json");

    std::string axiom;
    std::map<char, std::string> rules;
    float theta = 0.0f;

    if (!file.is_open()){
        cout << "file was not found or didnt open";
        return std::make_tuple(axiom, rules, theta);
    } 

    json parsedData;
    file >> parsedData;

    axiom = parsedData["axiom"].get<std::string>();
    theta = parsedData["theta"].get<float>();

    json rulesJson = parsedData["rules"];

    for(json::iterator it = rulesJson.begin(); it != rulesJson.end(); it++){
        std::string keyString = it.key();

        char keyChar = keyString[0];
        std::string ruleValue = it.value().get<std::string>();
        rules[keyChar] = ruleValue;
    }

    file.close();

    return std::make_tuple(axiom, rules, theta);
}

int main(int argc, char** argv){
    std::tuple<string, std::map<char, std::string>, float> data = parseJSON();
    std::string expanded = generateExpansion(data);

    float theta = std::get<2>(data);
    writeInstructionsToJSON(expanded, theta);
    
    return 0;
}