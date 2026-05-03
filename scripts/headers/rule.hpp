#ifndef RULEHPP
#define RULEHPP

#include <vector>
#include <string>


struct Rule{
    char LHS;
    std::vector<float> probs;
    std::vector<std::string> RHS;    
};


#endif