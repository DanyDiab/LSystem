#include <iostream>
#include <string>
#include <fstream>
#include <utility>
#include <nlohmann/json.hpp>
#include "./headers/tokens.hpp"
#include "./headers/rules.hpp"
#include <vector>
#include <sstream>

using namespace std;
using json = nlohmann::json;

int MAXDEPTH = 6;

std::vector<Token> recurExpand(std::vector<Token> curr, Rule** rules, int numRules, int depth){
    if(depth == MAXDEPTH) return curr;
    std::vector<Token> nextTokens;
    
    for(const auto& token : curr){
        bool foundExpansion = false;
        for(int i = 0 ; i < numRules; i++){
            Rule* rule = rules[i];
            if(rule->LHS == token){
                std::vector<Token> RHS = rule->RHS;
                for(const auto& rhsToken : RHS){
                    nextTokens.push_back(rhsToken);
                    foundExpansion = true;
                }
                break;
            }
        }
        if(!foundExpansion) nextTokens.push_back(token);
    }

    return recurExpand(nextTokens,rules,numRules, depth+1);
}

int writeInstructionsToJSON(std::vector<Token> tokens, int theta){
    json data;
    data["instructions"] = tokens;
    data["theta"] = theta;

    std::ofstream outputFile("./instructions.json");

    if(!outputFile.is_open()) return 1;

    outputFile << data.dump(4);
    outputFile.close();
    return 0;
}

std::vector<Token> generateExpansion(std::tuple<std::vector<Token>, Rule**, int, int> data){
    

    std::vector<Token> axiom = std::get<0>(data);
    Rule** rules = std::get<1>(data);
    int numRules = std::get<3>(data);


    return recurExpand(axiom,rules,numRules,0);
}

// <axiom, rules>
std::tuple<string, std::map<std::string, std::string>, int> parseJSON(){
    std::ifstream file("./system.json");

    std::string axiom;
    std::map<std::string, std::string> rules;
    int theta = 0;

    if (!file.is_open()){
        cout << "file was not found or didnt open";
        return std::make_tuple(axiom, rules, theta);
    } 

    json parsedData;
    file >> parsedData;

    axiom = parsedData["axiom"].get<std::string>();
    rules = parsedData["rules"].get<std::map<std::string, std::string>>();
    theta = parsedData["theta"].get<int>();

    file.close();

    return std::make_tuple(axiom, rules, theta);
}

std::vector<std::string> splitStringBySpace(std::string stringToSplit){
    std::vector<std::string> split;
    std::istringstream iss(stringToSplit);
    std::string token;

    while(iss >> token){
        split.push_back(token);
    }

    return split;
}

std::tuple<std::vector<Token>, Rule**, int, int> tokenize(std::tuple<std::string, std::map<std::string, std::string>, int> inputData) {
    std::string axiomString = std::get<0>(inputData);
    std::map<std::string, std::string> rules = std::get<1>(inputData);
    int theta = std::get<2>(inputData);

    std::vector<Token> axiomTokens;
    std::vector<std::string> axiomSplit = splitStringBySpace(axiomString);
// tokenize the axiom
    for(const auto& axiomTokenString : axiomSplit){
        Token axiomToken = tokenMap.at(axiomTokenString);
        axiomTokens.push_back(axiomToken);
    }
    
    // create rules
    Rule **tokenRules = new Rule*[rules.size()];
    
    int ruleCount = 0;
    for(const auto& [key,value] : rules){
        Rule *rule = new Rule;
        Token LHS = tokenMap.at(key);
        rule->LHS = LHS;
        // split RHS
        std::vector<std::string> RHSSplit = splitStringBySpace(value);
        std::vector<Token> RHS;
        // add each token
        for(const auto& strTok : RHSSplit){
            Token token = tokenMap.at(strTok);
            RHS.push_back(token);
        }
        rule->RHS = RHS;
        tokenRules[ruleCount] = rule;
        ruleCount++;
    }
    return std::make_tuple(axiomTokens,tokenRules,theta, rules.size());
}


int main(int argc, char** argv){
    std::tuple<string, std::map<std::string, std::string>, int> data = parseJSON();
    std::tuple<std::vector<Token>, Rule**, int, int> tokenizedData = tokenize(data);
    std::vector<Token> expanded = generateExpansion(tokenizedData);

    int theta = std::get<2>(tokenizedData);
    writeInstructionsToJSON(expanded, theta);
    
    return 0;
}