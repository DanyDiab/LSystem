#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <nlohmann/json.hpp>
#include <variant>
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
const std::vector<char> operators = {'*', '+', '-', '/', '^'};

std::unordered_map<char, float> generateParamMapping(std::vector<ParaInstruction*> curr, std::vector<Rule> rules){
    
    std::unordered_map<char, float> paramMapping;

    for(const auto& rule : rules){
        std::vector<std::variant<float, std::string>> params = rule.LHS->params;

        for(const auto& param : params){
            const std::string* strPtr = std::get_if<std::string>(&param);
            paramMapping[(*strPtr)[0]] = 0;
        }
    }

    for(const auto& ins : curr){
        std::unordered_map<char, float>::const_iterator it = paramMapping.find(ins->token);

        if(it == paramMapping.end()) continue;

        std::vector<std::variant<float, std::string>> params = ins->params;
        for(const auto& param : params){
            const float *val = std::get_if<float>(&param);
            paramMapping[ins->token] = *val;
        }
    }


    return paramMapping;
}


std::vector<ParaInstruction*> recurExpand(std::vector<ParaInstruction*> curr, std::vector<Rule> rules, int depth){
    if(depth == MAXDEPTH) return curr;
    std::unordered_map<char, float> paramMapping = generateParamMapping(curr, rules);

    std::vector<ParaInstruction*> nextExpansion;
    for(const auto& currIns : curr){

        bool foundExpansion = false;
        for(const auto& rule : rules){
            if(rule.LHS->token != currIns->token) continue;

            foundExpansion = true;
            float rand = static_cast<float>(std::rand() / static_cast<float>(RAND_MAX));

            std::vector<float> probs = rule.probs;
            std::vector<std::vector<ParaInstruction*>> out = rule.RHS;
            
            int size = probs.size();
            float runningProb = 0.0f;
            for(int i = 0; i < size; i++){
                runningProb += probs.at(i);

                if(rand >= runningProb) continue;
                
                std::vector<ParaInstruction*> selectedOut = out.at(i);
                std::vector<ParaInstruction*> cleanedOut;
                // convert parameters from string to float
                for(const auto& sOut : selectedOut){
                    for(const auto& param : sOut->params){
                        if(std::holds_alternative<float>(param)) continue;
                        
                        const std::string* strPtr = std::get_if<std::string>(&param);
                        std::vector<std::string> operatorSplit = Util::splitString(*strPtr, operators, true);
                        // no operator was present
                        if(operatorSplit.size() == 1){
                            float val = paramMapping[operatorSplit[0][0]];
                            sOut->params.pop_back();
                            sOut->params.push_back(val);

                            cleanedOut.push_back(sOut);
                        }
                        // operator was presesnt, must have 3 components
                        else{
                            float val1 = paramMapping[operatorSplit[0][0]];
                            float val2 = paramMapping[operatorSplit[2][0]];
                            sOut->params.pop_back();
                            
                            char op = operatorSplit[1][0];
                            float res;
                            switch(op){
                                case('*'):{
                                    res = val1 * val2;
                                    break;
                                }
                                case('+'):{
                                    res = val1 + val2;
                                    break;
                                }
                                case('/'):{
                                    res = val1 / val2;
                                    break;
                                }
                                case('^'):{
                                    res = std::pow(val1, val2);
                                }
                            }
                            sOut->params.push_back(res);
                            cleanedOut.push_back(sOut);
                        }
                    }

                }

                nextExpansion.emplace_back();
                break;
                
            }
        }
        if(!foundExpansion){
            nextExpansion.push_back(currIns);
        }
    }

    return recurExpand(nextExpansion,rules, depth+1);
}

std::vector<ParaInstruction*> generateExpansion(std::tuple<ParaInstruction*, std::vector<Rule>> data){
    ParaInstruction* axiom = std::get<0>(data);
    std::vector<ParaInstruction*> curr = {axiom}; 

    std::vector<Rule> rules = std::get<1>(data);


    return recurExpand(curr,rules, 0);
}

ParaInstruction* encodeInstruction(std::string instructionToEncode, std::unordered_map<std::string, float> constants){
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
            paramsVariant.push_back(paramF);
        }
        else{
            // split by operator
            std::vector<std::string> paramOpSplit = Util::splitString(param, operators, true);
            std::string fullParam = "";
            for(const auto& operand : paramOpSplit){
                std::unordered_map<std::string, float>::const_iterator it = constants.find(operand);
                if(it == constants.end()){
                    fullParam += operand;
                }
                else{
                    float replacement = it->second;
                    fullParam += std::to_string(replacement);
                }
            }
            paramsVariant.push_back(fullParam);
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
    std::unordered_map<std::string, float> constants;
    std::vector<ParaInstruction*> RHSVec;
    ParaInstruction* axiomIns = new ParaInstruction();
    if (!file.is_open()){
        cout << "file was not found or didnt open";
        return std::make_tuple(axiomIns, rules);
    } 

    json parsedData;
    file >> parsedData;

    std::string axiomStr = parsedData["axiom"].get<std::string>();
    constants = parsedData["constants"].get<std::unordered_map<std::string, float>>();


    axiomIns = encodeInstruction(axiomStr, constants);
    
    json rulesJson = parsedData["rules"];
    for(json::iterator it = rulesJson.begin(); it != rulesJson.end(); it++){
        Rule rule;
        std::string keyString = it.key();

        ParaInstruction* key = encodeInstruction(keyString, constants);

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
                ParaInstruction* paraIns = encodeInstruction(tok, constants);
                RHSVec.push_back(paraIns);
            }
            
        }

        rule.RHS = {RHSVec};
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
    std::vector<ParaInstruction*> expanded = generateExpansion(data);

    // writeInstructionsToJSON(expanded, theta);
    
    return 0;
}