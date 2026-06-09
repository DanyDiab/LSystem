#include <vector>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <string>
#include <tuple>
#include <nlohmann/json.hpp>
#include <iostream>

#include "./headers/tokens.hpp"
#include "./headers/rule.hpp"
#include "./headers/Util.hpp"
#include "./headers/encoder.hpp"
#include "./headers/JsonUtil.hpp"
#include <nlohmann/json_fwd.hpp>


using namespace std;
using json = nlohmann::json;

const int defaultMaxDepth = 10;

// <axiom, rules>
std::tuple<std::vector<ParaInstruction*>, std::vector<Rule>, int> parseJSON(const string inFile){
    std::ifstream file(inFile);

    std::vector<Rule> rules;
    std::unordered_map<std::string, float> constants;
    std::vector<ParaInstruction*> axiomIns;

    int maxDepth = defaultMaxDepth;
    if (!file.is_open()){
        cout << "file was not found or didnt open";
        return std::make_tuple(axiomIns, rules, maxDepth);
    } 

    json parsedData;
    file >> parsedData;

    std::string axiomStr = parsedData["axiom"].get<std::string>();
    constants = parsedData["constants"].get<std::unordered_map<std::string, float>>();
// extract max dpeth if it exists
    if(constants.find("MaxDepth") != constants.end()){
        maxDepth = constants["MaxDepth"];
        constants.erase("MaxDepth");
    }

    axiomIns = stringToInsVec(axiomStr, constants);
    
    json rulesJson = parsedData["rules"];
    for(json::iterator it = rulesJson.begin(); it != rulesJson.end(); it++){
        Rule rule;
        std::string keyString = it.key();

        ParaInstruction* key = encodeInstruction(keyString, constants, operators);

        rule.LHS = key;

        json outcomes = it.value();

        std::vector<float> probs;
        std::vector<std::string> outs;
        for(const auto& outcome : outcomes){
            float prob = outcome["prob"].get<float>();
            
            probs.push_back(prob);
            
            std::string out = outcome["out"].get<std::string>();
            
            std::vector<ParaInstruction*> insVec = stringToInsVec(out, constants);
            
            rule.RHS.push_back(insVec);
            
        }

        rule.probs = probs;
        rules.push_back(rule);
    }

    file.close();

    return std::make_tuple(axiomIns, rules, maxDepth);
}


int writeInstructionsToJSON(std::vector<ParaInstruction*> expanded, const string outFile) {
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




