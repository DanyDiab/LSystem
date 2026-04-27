#ifndef RULES_H
#define RULES_H

#include <iostream>
#include <string>
#include <vector>
#include "tokens.hpp"

struct Rule{
    Token LHS;
    std::vector<Token> RHS;
};

#endif