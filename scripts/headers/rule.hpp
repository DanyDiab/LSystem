#ifndef RULEHPP
#define RULEHPP

#include <vector>



struct Rule{
    char LHS;
    std::vector<float> prob;
    std::vector<char> RHS;    
};


#endif