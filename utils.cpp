#include "utils.h"
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> split(const std::string &str)
{
    std::vector<std::string> words;
    std::stringstream ss(str);
    std::string word;
    while (ss >> word)
    {
        words.push_back(word);
    }
    return words;
}
