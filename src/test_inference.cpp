#include <iostream>
#include <string>
#include <vector>
#include "drakon.h"
#include "tokenizer.h"

int main() {
    try {
        std::string model_path = "build/dummy_model.bin";
        std::string prompt = "hello world";

        std::cout << "[TEST] Loading model\n";
        DrakonModel model(model_path);

        std::cout << "[TEST] Tokenizing\n";
        Tokenizer tok;
        auto tokens = tok.encode(prompt);

        std::cout << "[TEST] Forward pass over " << tokens.size() << " tokens\n";
        for (int t : tokens) {
            float y = model.forward(t);
            std::cout << "[TEST] token=" << t << " -> " << y << "\n";
        }

        std::cout << "[TEST] OK\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "[TEST] FAILED: " << e.what() << "\n";
        return 1;
    }
}
