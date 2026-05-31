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

int MAXDEPTH = 10;
const std::vector<char> operators = {'*', '+', '-', '/', '^'};
const std::string inFile = "./systems/paraSystem.json";
const std::string outFile = "./instructions.json";

std::unordered_map<char, float> generateParamMapping(std::vector<ParaInstruction*> curr, std::vector<Rule> rules){
    
    std::unordered_map<char, float> paramMapping;
    std::unordered_map<char,int> seenTokens;

    std::vector<char> keys;
    std::vector<float> values;

    for(const auto& rule : rules){
        std::vector<std::variant<float, std::string>> params = rule.LHS->params;
        seenTokens[rule.LHS->token] = 1;
        for(const auto& param : params){
            const std::string* strPtr = std::get_if<std::string>(&param);
            char token = (*strPtr)[0];
            keys.push_back(token);
            
        }
    }

    for(const auto& ins : curr){
        std::unordered_map<char, int>::const_iterator it = seenTokens.find(ins->token);

        if(it == seenTokens.end()) continue;

        std::vector<std::variant<float, std::string>> params = ins->params;
        for(const auto& param : params){
            const float *val = std::get_if<float>(&param);
            values.push_back(*val);
        }
    }

    for(int i = 0; i < keys.size(); i++){
        char currKey = keys.at(i);
        float currVal = values.at(i);

        paramMapping[currKey] = currVal;
    }


    return paramMapping;
}


void parseOperations(std::vector<std::string> operatorSplit, std::unordered_map<char, float> paramMapping, ParaInstruction* sOutPtr, auto arrIdx){
    float val1, val2;

    if(Util::isNumeric(operatorSplit[0])){
        val1 = std::stof(operatorSplit[0]);
    }
    else{
        val1 = paramMapping[operatorSplit[0][0]];
    }
    if(Util::isNumeric(operatorSplit[2])){
        val2 = std::stof(operatorSplit[2]);
    }
    else{
        val2 =  paramMapping[operatorSplit[2][0]];
    }

    sOutPtr->params.erase(arrIdx);

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
            break;
        }
    }
    sOutPtr->params.insert(arrIdx, res);
}

void cleanParams(std::unordered_map<char, float> paramMapping, ParaInstruction* sOutPtr){
    int paramCount = 0;

    for(const auto& param : sOutPtr->params){

        if(std::holds_alternative<float>(param)) continue;
            
        auto arrIdx = sOutPtr->params.begin() + paramCount;

        const std::string* strPtr = std::get_if<std::string>(&param);
        std::vector<std::string> operatorSplit = Util::splitString(*strPtr, operators, true);
        // no operator was present
        if(operatorSplit.size() == 1){
            float val = paramMapping[operatorSplit[0][0]];
            sOutPtr->params.erase(arrIdx);
            sOutPtr->params.insert(arrIdx, val);
        }
        // operator was presesnt, must have 3 components
        else{
            parseOperations(operatorSplit,paramMapping,sOutPtr,arrIdx);
        }
        paramCount++;
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
    }

    return selectedOut;
}

// for each instruciton
// if params contains string, create mapping 

std::vector<ParaInstruction*> recurExpand(std::vector<ParaInstruction*> curr, std::vector<Rule> rules, int depth){
    if(depth == MAXDEPTH) return curr;
    std::unordered_map<char, float> paramMapping = generateParamMapping(curr, rules);

    std::vector<ParaInstruction*> nextExpansion;
    for(const auto& currIns : curr){

        bool foundExpansion = false;
        for(const auto& rule : rules){
            if(rule.LHS->token != currIns->token) continue;

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

std::vector<ParaInstruction*> generateExpansion(std::tuple<ParaInstruction*, std::vector<Rule>> data){
    ParaInstruction* axiom = std::get<0>(data);
    std::vector<ParaInstruction*> curr = {new ParaInstruction(*axiom)}; 

    std::vector<Rule> rules = std::get<1>(data);


    return recurExpand(curr,rules, 0);
}

ParaInstruction* encodeInstruction(std::string instructionToEncode, std::unordered_map<std::string, float> constants){
    ParaInstruction* paraIns = new ParaInstruction();
    
    std::vector<char> delim = {'('};
    std::vector<char> paramDelim = {',', ')', ' '};

    std::vector<std::variant<float, std::string>> paramsVariant;

    std::vector<std::string> split = Util::splitString(instructionToEncode, delim);

    char token = split[0][0];

    if(split.size() == 1){
        paraIns->token = token;
        paraIns->params = paramsVariant;

        return paraIns;
    }
    std::string params = split[1];
    std::vector<std::string> paramSplit = Util::splitString(params, paramDelim);

    for(const auto& param : paramSplit){
        if(Util::isNumeric(param)){
            float paramF = std::stof(param);
            paramsVariant.push_back(paramF);
        }
        else{
            // split by operator
            std::vector<std::string> paramOpSplit = Util::splitString(param, operators, true);
            std::string fullParam = "";
            bool skipped = false;
            for(const auto& operand : paramOpSplit){
                std::unordered_map<std::string, float>::const_iterator it = constants.find(operand);
                if(it == constants.end()){
                    fullParam += operand;
                }
                else{
                    float replacement = it->second;
                    // if this is the only parameter, dont convert to a string
                    if(paramOpSplit.size() == 1){
                        skipped = true;
                        paramsVariant.push_back(replacement);
                        break;
                    }
                    fullParam += std::to_string(replacement);
                }
            }
            if(!skipped){
                paramsVariant.push_back(fullParam);
            }
        }
    }

    paraIns->token = token;
    paraIns->params = paramsVariant;
    return paraIns;

}

// <axiom, rules>
std::tuple<ParaInstruction*, std::vector<Rule>> parseJSON(){
    std::ifstream file(inFile);

    std::vector<Rule> rules;
    std::unordered_map<std::string, float> constants;
    std::vector<ParaInstruction*> RHSVec;
    ParaInstruction* axiomIns;
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
            rule.RHS.push_back(RHSVec);
            RHSVec.clear();
            
        }

        rule.probs = probs;
        rules.push_back(rule);
    }

    file.close();

    return std::make_tuple(axiomIns, rules);
}

int writeInstructionsToJSON(std::vector<ParaInstruction*> expanded) {
    json data;
    json jsonTokens = json::array();

    for (ParaInstruction* instr : expanded) {
        json instructionJson;
        
        std::string tokenStr(1, instr->token);
        instructionJson["token"] = tokenStr;

        json paramsJson = json::array();
        
        for (std::variant<float, std::string>& param : instr->params) {
            if (std::holds_alternative<float>(param)) {
                paramsJson.push_back(std::get<float>(param));
            } else if (std::holds_alternative<std::string>(param)) {
                paramsJson.push_back(std::get<std::string>(param));
            }
        }
        
        instructionJson["params"] = paramsJson;
        jsonTokens.push_back(instructionJson);
    }

    data["instructions"] = jsonTokens;

    std::ofstream outputFile(outFile);

    if (!outputFile.is_open()) return 1;

    outputFile << data.dump(4);
    outputFile.close();
    
    return 0;
}

int main(int argc, char** argv){
    unsigned int currentTime = static_cast<unsigned int>(time(nullptr));
    srand(currentTime);
    std::tuple<ParaInstruction*, std::vector<Rule>> data = parseJSON();
    std::vector<ParaInstruction*> expanded = generateExpansion(data);

    writeInstructionsToJSON(expanded);
    
    for(const auto& ins : expanded){
        delete ins;
    }

    std::vector<Rule>& rules = std::get<1>(data);
    for(auto& rule : rules) {
        delete rule.LHS;
        
        for(auto& probBranch : rule.RHS) {
            for(auto& ins : probBranch) {
                delete ins;
            }
        }
    }

    delete std::get<0>(data);

    return 0;
}