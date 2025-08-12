#include "drakon.h"
#include "model_loader.h"
#include <iostream>

DrakonModel::DrakonModel(const std::string& model_path) {
    std::cout << "[🧠] Initializing DrakonModel with path: " << model_path << std::endl;
    ModelLoader loader;
    weights = loader.load(model_path);
}

float DrakonModel::forward(int token) {
    if (weights.mock_weights.empty()) {
        std::cerr << "[⚠️] No weights loaded — returning 0.\n";
        return 0.0f;
    }

    // Dummy forward pass: weighted sum * token
    float sum = 0.0f;
    for (float w : weights.mock_weights) {
        sum += w;
    }
    return sum * token;
}
