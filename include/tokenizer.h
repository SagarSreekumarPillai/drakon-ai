#pragma once
#include <string>
#include <vector>

class Tokenizer {
public:
    Tokenizer();
    std::vector<int> encode(const std::string& prompt);
};
