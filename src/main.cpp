#include "drakon.h"
#include "tokenizer.h"
#include "model_loader.h"
#include "backend.h"

#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: drakon <model-path> <prompt>\n";
        return 1;
    }

    std::string model_path = argv[1];
    std::string prompt = argv[2];

    // Step 1: Load the model
    std::cout << "[🔍] Loading model from: " << model_path << std::endl;
    DrakonModel model(model_path);

    // Step 2: Tokenize input
    std::cout << "[✂️] Tokenizing prompt...\n";
    Tokenizer tokenizer;
    std::vector<int> tokens = tokenizer.encode(prompt);

    // Step 3: Run inference (simplified loop)
    std::cout << "[⚙️] Running inference...\n";
    for (int token : tokens) {
        float prediction = model.forward(token);  // placeholder
        std::cout << "[📤] Predicted value (mock): " << prediction << std::endl;
    }

    std::cout << "[✅] Done.\n";
    return 0;
}
