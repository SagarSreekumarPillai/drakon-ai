// src/drakon.cpp

#include "drakon.h"
#include "model_loader.h"
#include "backend.h"
#include <iostream>
#include <vector>
#include <string>

DrakonModel::DrakonModel(const std::string& model_path) {
    std::cout << "[🧠] DrakonModel constructor: loading model from " << model_path << std::endl;
    DrakonWeights weights = ModelLoader::load(model_path);
    backend_ = new DrakonBackend(weights.mock_weights);
}

float DrakonModel::forward(int token) {
    return backend_->forward(token);
}

std::vector<int> tokenize(const std::string& prompt) {
    std::cout << "[✂️] Tokenizing prompt..." << std::endl;
    std::vector<int> tokens;
    for (char c : prompt) {
        tokens.push_back(static_cast<int>(c));
    }
    std::cout << "[📦] Tokenizer initialized." << std::endl;
    std::cout << "[🔢] Encoded " << tokens.size() << " tokens." << std::endl;
    return tokens;
}

int run_drakon_cli(int argc, char** argv) {
    std::string model_path = argv[1];
    std::string prompt = argv[2];

    DrakonModel model(model_path);
    std::vector<int> tokens = tokenize(prompt);

    std::cout << "[⚙️] Running inference..." << std::endl;
    for (int token : tokens) {
        model.forward(token);
    }

    std::cout << "[✅] Done." << std::endl;
    return 0;
}
