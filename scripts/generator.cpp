#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <variant>
#include <sstream>
#include <map>
#include <cstdlib>
#include <stdexcept>
#include "./headers/Util.hpp"
#include "./headers/tokens.hpp"
#include "./headers/rule.hpp"
#include "./headers/JsonUtil.hpp"
#include "headers/Config.hpp"

using namespace std;

int MAXDEPTH = 10;
const std::vector<char> operators = {'*', '+', '-', '/', '^'};
const std::string inFile = "./systems/2.7/4.json";
const std::string outFile = "./instructions.json";

std::unordered_map<char, float> generateParamMapping(ParaInstruction* ins, std::vector<Rule> rules){
    
    std::unordered_map<char, float> paramMapping;

    char insTok = ins->token;

    for(const auto& rule : rules){
        if(rule.LHS->token != insTok) continue;
        
        std::vector<std::variant<float, std::string>> ruleParams = rule.LHS->params;
        std::vector<std::variant<float,  std::string>> insParams = ins->params;
        for(int i = 0; i < ruleParams.size(); i++){

            std::variant<float, std::string> ruleParam = ruleParams.at(i);
            std::variant<float, std::string> insParam = insParams.at(i);

            const std::string* strPtr = std::get_if<std::string>(&ruleParam);
            char key = (*strPtr)[0];

            const float *val = std::get_if<float>(&insParam);
            
            paramMapping[key] = *val;
        }
    }

    return paramMapping;
}


void cleanParams(std::unordered_map<char, float> paramMapping, ParaInstruction* sOutPtr) {
    for (size_t i = 0; i < sOutPtr->params.size(); i++) {
        
        if (std::holds_alternative<float>(sOutPtr->params[i])) {
            continue; 
        }

        const std::string* strPtr = std::get_if<std::string>(&sOutPtr->params[i]);
        std::vector<std::string> operatorSplit = Util::splitString(*strPtr, operators, true);
        
        if (operatorSplit.size() == 1) {
            sOutPtr->params[i] = paramMapping[operatorSplit[0][0]];
            continue;
        }

        float val1;
        if (!Util::isNumeric(operatorSplit[0])) {
            val1 = paramMapping[operatorSplit[0][0]];
        } else {
            val1 = std::stof(operatorSplit[0]);
        }

        float val2;
        if (!Util::isNumeric(operatorSplit[2])) {
            val2 = paramMapping[operatorSplit[2][0]];
        } else {
            val2 = std::stof(operatorSplit[2]);
        }

        char op = operatorSplit[1][0];
        float res = 0.0f;
        
        switch (op) {
            case '*': { res = val1 * val2; break; }
            case '+': { res = val1 + val2; break; }
            case '-': { res = val1 - val2; break; } 
            case '/': { res = val1 / val2; break; }
            case '^': { res = std::pow(val1, val2); break; }
        }
        
        sOutPtr->params[i] = res;
    }
}


std::vector<ParaInstruction*> selectStochRHS(const Rule* rule){

    float rand = static_cast<float>(std::rand() / static_cast<float>(RAND_MAX));

    std::vector<float> probs = rule->probs;
    std::vector<std::vector<ParaInstruction*>> out = rule->RHS;

    float runningProb = 0.0f;
    std::vector<ParaInstruction*> selectedOut;

    for(int i = 0; i < probs.size(); i++){
        runningProb += probs.at(i);

        if(rand >= runningProb) continue;
        
        selectedOut = out.at(i);
        break;
    }

    return selectedOut;
}


std::vector<ParaInstruction*> recurExpand(std::vector<ParaInstruction*> curr, std::vector<Rule> rules, int depth){
    if(depth == MAXDEPTH) return curr;


    std::vector<ParaInstruction*> nextExpansion;
    for(const auto& currIns : curr){

        bool foundExpansion = false;
        for(const auto& rule : rules){
            if(rule.LHS->token != currIns->token) continue;
            
            std::unordered_map<char, float> paramMapping = generateParamMapping(currIns, rules);

            foundExpansion = true;

            std::vector<ParaInstruction*> selectedOut = selectStochRHS(&rule);

            std::vector<ParaInstruction*> cleanedOut;
            // convert parameters from string to float
            for(const auto& sOut : selectedOut){
                ParaInstruction* sOutPtr = new ParaInstruction(*sOut);
                cleanParams(paramMapping, sOutPtr);
                cleanedOut.push_back(sOutPtr);
            }
            nextExpansion.insert(nextExpansion.end(), cleanedOut.begin(), cleanedOut.end());
        }
        if(!foundExpansion){
            nextExpansion.push_back(currIns);
        }
        else{
            delete currIns;
        }
    }

    return recurExpand(nextExpansion,rules, depth+1);
}

std::vector<ParaInstruction*> generateExpansion(std::shared_ptr<LSystemConfig> data){
    std::vector<ParaInstruction*> axiom = data->axiomIns;

    std::vector<ParaInstruction*> curr(axiom); 

    std::vector<Rule> rules = data->rules;


    return recurExpand(curr,rules, 0);
}

void cleanUp(std::shared_ptr<LSystemConfig> data, std::vector<ParaInstruction*> expanded){
    for(const auto& ins : expanded){
        delete ins;
    }
    std::vector<Rule>& rules = data->rules;
    for(auto& rule : rules) {
        delete rule.LHS;
        
        for(auto& probBranch : rule.RHS) {
            for(auto& ins : probBranch) {
                delete ins;
            }
        }
    }
}

int main(int argc, char** argv){
    unsigned int currentTime = static_cast<unsigned int>(time(nullptr));
    srand(currentTime);
   
    std::shared_ptr<LSystemConfig> data = parseJSON(inFile);

    MAXDEPTH = data->maxDepth;

    std::vector<ParaInstruction*> expanded = generateExpansion(data);
    
    writeInstructionsToJSON(expanded, data, outFile);

    cleanUp(data,expanded);

    return 0;
}