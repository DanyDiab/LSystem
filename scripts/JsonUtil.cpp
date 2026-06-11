#include <memory>
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
#include "./headers/Config.hpp"
#include <nlohmann/json_fwd.hpp>


using namespace std;
using json = nlohmann::json;

const int defaultMaxDepth = 10;

// <axiom, rules>
std::shared_ptr<LSystemConfig> parseJSON(const string inFile){
    std::ifstream file(inFile);

    std::vector<Rule> rules;
    std::unordered_map<std::string, float> constants;
    std::unordered_map<std::string, float> globals;
    
    std::vector<ParaInstruction*> axiomIns;
    std::shared_ptr<LSystemConfig> config = std::make_shared<LSystemConfig>();
    
    config->maxDepth = defaultMaxDepth;
    std::vector<float> tropismDir;
    float tropsimSusceptibility;
    if (!file.is_open()){
        cout << "file was not found or didnt open";
        return config;
    } 

    json parsedData;
    file >> parsedData;

    std::string axiomStr = parsedData["axiom"].get<std::string>();
    constants = parsedData["constants"].get<std::unordered_map<std::string, float>>();
    globals = parsedData["globals"].get<std::unordered_map<std::string, float>>();

// extract max dpeth if it exists
    if(globals.find("MaxDepth") != constants.end()){
        config->maxDepth = globals["MaxDepth"];
    }
    if(globals.find("TropismX") != globals.end()){
        float x = globals["TropismX"];
        float y = globals["TropismY"];
        float z = globals["TropismZ"];

        config->tropismDir = {x,y,z};

        config->tropismSusceptibility = globals["tropismSusceptibility"];

    }

    config->axiomIns = stringToInsVec(axiomStr, constants);

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
    config->rules = rules;

    file.close();

    return config;
}


int writeInstructionsToJSON(std::vector<ParaInstruction*> expanded, shared_ptr<LSystemConfig> config, const string outFile) {
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
    
    data["tropism"]["direction"] = { config->tropismDir.x, config->tropismDir.y, config->tropismDir.z };
    data["tropism"]["susceptibility"] = config->tropismSusceptibility;

    std::ofstream outputFile(outFile);

    if (!outputFile.is_open()) return 1;

    outputFile << data.dump(4);
    outputFile.close();
    
    return 0;
}




