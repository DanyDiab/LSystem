#pragma once
#include <string>
#include <vector>

namespace Util {
    std::vector<std::string> splitString(const std::string& stringToSplit, const std::vector<char>& delims, bool keepDelims = false);
    std::vector<std::string> tokenize(const std::string& stringToSplit);
    bool isNumeric(const std::string& text);
    std::vector<std::string> tokenize(const std::string& stringToSplit);
    void ReplaceAll(std::string& text, const std::string& toSearch, const std::string& toReplace);
}