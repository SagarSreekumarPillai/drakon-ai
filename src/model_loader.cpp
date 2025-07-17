// src/model_loader.cpp

#include "model_loader.h"
#include <iostream>

DrakonWeights ModelLoader::load(const std::string& path) {
    std::cout << "[🔍] Loading model from: " << path << std::endl;

    // Return dummy weights
    DrakonWeights weights;
    weights.mock_weights = {1.1f, 2.2f, 3.3f, 4.4f};  // Just mock values
    return weights;
}
