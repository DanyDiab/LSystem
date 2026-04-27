#include <iostream>
#include <string>
#include <fstream>
#include <utility>
#include "./headers/imports/json.hpp"
#include "./headers/tokens.hpp"
#include "./headers/rules.hpp"
#include <vector>
#include <sstream>

using namespace std;
using json = nlohmann::json;

int MAXDEPTH = 2;

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

std::vector<Token> generateExpansion(std::tuple<Token, Rule**, int, int> data){
    

    Token axiom = std::get<0>(data);
    Rule** rules = std::get<1>(data);
    int numRules = std::get<3>(data);

    std::vector<Token> curr = {axiom};

    return recurExpand(curr,rules,numRules,0);
}

// <axiom, rules>
std::tuple<string, std::map<std::string, std::string>, int> parseJSON(){
    std::ifstream file("./system.json");
    std::string line;

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

std::tuple<Token, Rule**, int, int> tokenize(std::tuple<std::string, std::map<std::string, std::string>, int> inputData) {
    std::string axiomString = std::get<0>(inputData);
    std::map<std::string, std::string> rules = std::get<1>(inputData);
    int theta = std::get<2>(inputData);


    Token axiomToken = tokenMap.at(axiomString);
    
    Rule **tokenRules = new Rule*[rules.size()];
    
    int ruleCount = 0;
    for(const auto& [key,value] : rules){
        Rule *rule = new Rule;
        Token LHS = tokenMap.at(key);
        rule->LHS = LHS;

        std::vector<std::string> RHSSplit = splitStringBySpace(value);
        std::vector<Token> RHS;
        for(const auto& strTok : RHSSplit){
            Token token = tokenMap.at(strTok);
            RHS.push_back(token);
        }
        rule->RHS = RHS;
        tokenRules[ruleCount] = rule;
        ruleCount++;
    }
    return std::make_tuple(axiomToken,tokenRules,theta, rules.size());
}


void printTokenizedData(const std::tuple<Token, Rule**, int, int>& tokenizedData) {
    Token axiom = std::get<0>(tokenizedData);
    Rule** rules = std::get<1>(tokenizedData);
    int theta = std::get<2>(tokenizedData);
    int numRules = std::get<3>(tokenizedData);

    std::cout << "Axiom: " << static_cast<int>(axiom) << "\n";
    std::cout << "Theta: " << theta << "\n";


    for(int i = 0; i < numRules; i++) {
        Rule* currRule = rules[i];

        Token LHS = currRule->LHS;
        std::vector<Token> RHS = currRule->RHS;

        std::cout << "LHS: " << static_cast<int>(LHS) << " -> RHS: [";
        
        for (size_t j = 0; j < RHS.size(); j++) {
            std::cout << static_cast<int>(RHS[j]);
            
            if (j < RHS.size() - 1) {
                std::cout << ", ";
            }
        }
        
        std::cout << "]\n";
    }    
}


int main(int argc, char** argv){
    std::tuple<string, std::map<std::string, std::string>, int> data = parseJSON();
    std::tuple<Token, Rule**, int, int> tokenizedData = tokenize(data);
    printTokenizedData(tokenizedData);
    std::vector<Token> expanded = generateExpansion(tokenizedData);

    for(const auto& token : expanded){
        std::cout << "token: " << static_cast<int>(token) << "\n";
    }
    return 0;
}


// read in JSON
// convert to a token Axoim and an array of Rules
// recursivly match the current token to the LHS of rules, when matching, swap it with RHS
// continue until a certain depth is reached