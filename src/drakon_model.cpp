#include "drakon.h"
#include "model_loader.h"
#include <iostream>

DrakonModel::DrakonModel(const std::string& model_path) {
    std::cout << "[🧠] Initializing DrakonModel with path: " << model_path << std::endl;
    ModelLoader loader;
    weights = loader.load(model_path);
}

float DrakonModel::forward(int token) {
    float sum = 0.0f;
    for (float w : weights.mock_weights) {
        sum += w;
    }
    return sum * token;  // dummy math
}

