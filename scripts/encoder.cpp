#include <unordered_map>
#include <vector>

#include "./headers/encoder.hpp"
#include "./headers/rule.hpp"
#include "./headers/tokens.hpp"
#include "./headers/Util.hpp"

ParaInstruction* encodeInstruction(std::string instructionToEncode, std::unordered_map<std::string, float> constants, const std::vector<char> operators){
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

std::vector<ParaInstruction*> stringToInsVec(std::string toEncode, std::unordered_map<std::string, float> constants){
    std::vector<ParaInstruction*> vec = std::vector<ParaInstruction*>();
    std::vector<std::string> outTokens = Util::tokenize(toEncode);

    for(const auto& tok : outTokens){
        ParaInstruction* paraIns = encodeInstruction(tok, constants, operators);
        vec.push_back(paraIns);
    }
    return vec;
}
