#include <iostream>
#include <string>
#include <fstream>
#include <utility>
#include <nlohmann/json.hpp>
#include <vector>
#include <sstream>
#include <map>
#include <cstdlib>
#include <stdexcept>


#include "./headers/Util.hpp"
#include "./headers/tokens.hpp"
#include "./headers/rule.hpp"

using namespace std;
using json = nlohmann::json;

int MAXDEPTH = 5;

// std::string recurExpand(std::string curr, std::vector<Rule> rules, int numRules, int depth){
//     if(depth == MAXDEPTH) return curr;
//     std::string nextExpansion;
    
//     for(const auto& character : curr){
//         if(std::isspace(character)) continue;

//         bool foundExpansion = false;
//         for(const auto& rule : rules){
//             if(rule.LHS != character) continue;

//             foundExpansion = true;
//             float rand = static_cast<float>(std::rand() / static_cast<float>(RAND_MAX));

//             std::vector<float> probs = rule.probs;
//             std::vector<string> out = rule.RHS;
            
//             int size = probs.size();
//             float runningProb = 0.0f;
//             for(int i = 0; i < size; i++){
//                 runningProb += probs.at(i);

//                 if(rand >= runningProb) continue;
                
//                 nextExpansion += out.at(i);
//                 break;
                
//             }
//             break;
//         }
//         if(!foundExpansion){
//             nextExpansion += character;
//         }
//     }

//     return recurExpand(nextExpansion,rules,numRules, depth+1);
// }

// std::string generateExpansion(std::tuple<string, std::vector<Rule>, float> data){
//     std::string axiom = std::get<0>(data);
//     std::vector<Rule> rules = std::get<1>(data);
//     int numRules = rules.size();
//     return recurExpand(axiom,rules,numRules,0);
// }

ParaInstruction* encodeInstruction(std::string instructionToEncode){
    ParaInstruction* paraIns = new ParaInstruction();
    
    std::vector<char> delim = {'('};
    std::vector<char> paramDelim = {',', ')', ' '};

    std::vector<std::string> split = Util::splitString(instructionToEncode, delim);

    char token = split[0][0];

    std::string params = split[1];
    std::vector<std::string> paramSplit = Util::splitString(params,paramDelim);

    std::vector<std::variant<float, std::string>> paramsVariant;


    for(const auto& param : paramSplit){
        if(Util::isNumeric(param)){
            float paramF = std::stof(param);
            cout << "this is a float: " << paramF << "\n";
            paramsVariant.push_back(paramF);
        }
        else{
            cout << "this is a string: " << param << "\n";
            paramsVariant.push_back(param);
        }
    }

    paraIns->token = token;
    paraIns->params = paramsVariant;
    return paraIns;

}

// <axiom, rules>
std::tuple<ParaInstruction*, std::vector<Rule>> parseJSON(){
    std::ifstream file("./systems/paraSystem.json");

    std::vector<Rule> rules;
    std::map<std::string, float> constants;
    std::vector<ParaInstruction*> RHSVec;
    ParaInstruction* axiomIns = new ParaInstruction();
    if (!file.is_open()){
        cout << "file was not found or didnt open";
        return std::make_tuple(axiomIns, rules);
    } 

    json parsedData;
    file >> parsedData;

    std::string axiomStr = parsedData["axiom"].get<std::string>();

    axiomIns = encodeInstruction(axiomStr);
    constants = parsedData["constants"].get<std::map<std::string, float>>();
    
    json rulesJson = parsedData["rules"];
    for(json::iterator it = rulesJson.begin(); it != rulesJson.end(); it++){
        Rule rule;
        std::string keyString = it.key();

        ParaInstruction* key = encodeInstruction(keyString);

        rule.LHS = key;

        json outcomes = it.value();

        std::vector<float> probs;
        std::vector<std::string> outs;
        for(const auto& outcome : outcomes){
            float prob = outcome["prob"].get<float>();
            
            probs.push_back(prob);
            
            
            std::string out = outcome["out"].get<std::string>();
            
            std::vector<std::string> outTokens = Util::tokenize(out);

            for(const auto& tok : outTokens){
                ParaInstruction* paraIns = encodeInstruction(tok);
                RHSVec.push_back(paraIns);
            }
            
        }
        rule.RHS = RHSVec;
        rule.probs = probs;
        rules.push_back(rule);
    }

    file.close();

    return std::make_tuple(axiomIns, rules);
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

int main(int argc, char** argv){
    unsigned int currentTime = static_cast<unsigned int>(time(nullptr));
    srand(currentTime);
    std::tuple<ParaInstruction*, std::vector<Rule>> data = parseJSON();
    // std::string expanded = generateExpansion(data);

    // float theta = std::get<2>(data);
    // writeInstructionsToJSON(expanded, theta);
    
    return 0;
}