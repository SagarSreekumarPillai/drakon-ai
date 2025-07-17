#include "model_loader.h"
#include "tokenizer.h"
#include "backend.h"
#include "drakon.h"
#include <iostream>

// Simple constructor, can later load vocab if needed
Tokenizer::Tokenizer() {
    std::cout << "[📦] Tokenizer initialized.\n";
}

// Naive whitespace tokenizer: one word = one token ID (hashed)
std::vector<int> Tokenizer::encode(const std::string& prompt) {
    std::vector<int> tokens;
    std::string word;
    
    for (char c : prompt) {
        if (isspace(c)) {
            if (!word.empty()) {
                tokens.push_back(std::hash<std::string>{}(word) % 10000); // simulate token ID
                word.clear();
            }
        } else {
            word += c;
        }
    }

    if (!word.empty()) {
        tokens.push_back(std::hash<std::string>{}(word) % 10000);
    }

    std::cout << "[🔢] Encoded " << tokens.size() << " tokens.\n";
    return tokens;
}
