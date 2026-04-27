#ifndef RULES_H
#define RULES_H

#include <iostream>
#include <string>
#include "tokens.hpp"

struct Rule{
    Token LHS;
    Token* RHS;
};

#endif