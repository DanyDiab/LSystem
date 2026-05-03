#include <iostream>
#include <string>
#include <fstream>
#include <utility>
#include <nlohmann/json.hpp>
#include <vector>
#include <sstream>
#include <map>
#include "./headers/tokens.hpp"
#include "./headers/rule.hpp"
#include <cstdlib>

using namespace std;
using json = nlohmann::json;

int MAXDEPTH = 10;

std::string recurExpand(std::string curr, std::vector<Rule> rules, int numRules, int depth){
    if(depth == MAXDEPTH) return curr;
    std::string nextExpansion;
    
    for(const auto& character : curr){
        if(std::isspace(character)) continue;
        bool foundExpansion = false;
        for(const auto& rule : rules){
            if(rule.LHS == character){
                foundExpansion = true;
                float rand = static_cast<float>(std::rand() / RAND_MAX);

                std::vector<float> probs = rule.probs;
                std::vector<string> out = rule.RHS;
                
                int size = probs.size();
                float runningProb = 0.0f;
                for(int i = 0; i < size; i++){
                    runningProb += probs.at(i);
                    cout << "prob" << runningProb << "rand" << rand << "\n";
                    if(rand < runningProb){

                        nextExpansion += out.at(i);
                        break;
                    }
                }
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

std::string generateExpansion(std::tuple<string, std::vector<Rule>, float> data){
    std::string axiom = std::get<0>(data);
    std::vector<Rule> rules = std::get<1>(data);
    int numRules = rules.size();
    return recurExpand(axiom,rules,numRules,0);
}

// <axiom, rules>
std::tuple<string, std::vector<Rule>, float> parseJSON(){
    std::ifstream file("./system.json");

    std::string axiom;
    std::vector<Rule> rules;
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
        Rule rule;
        std::string keyString = it.key();
        char keyChar = keyString[0];

        rule.LHS = keyChar;

        json outcomes = it.value();

        std::vector<float> probs;
        std::vector<std::string> outs;
        for(const auto& outcome : outcomes){

            float prob = outcome["prob"].get<float>();
            std::string out = outcome["out"].get<std::string>();

            probs.push_back(prob);
            outs.push_back(out);
        }

        rule.probs = probs;
        rule.RHS = outs;
    }

    file.close();

    return std::make_tuple(axiom, rules, theta);
}

int main(int argc, char** argv){
    std::tuple<string, std::vector<Rule>, float> data = parseJSON();
    std::string expanded = generateExpansion(data);

    float theta = std::get<2>(data);
    writeInstructionsToJSON(expanded, theta);
    
    return 0;
}