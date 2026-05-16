#pragma once

#include <vector>
#include <string>
#include <variant>




// individual instrucitons in the system get parsed down to this
// float if it is constant
// char if it is a parameter of the rule
struct ParaInstruction{
    char token;
    std::vector<std::variant<float, std::string>> params;
};

struct Rule{
    ParaInstruction* LHS;
    std::vector<float> probs;
    std::vector<ParaInstruction*> RHS;    
};