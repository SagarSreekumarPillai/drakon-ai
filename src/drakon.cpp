// src/drakon.cpp

#include "drakon.h"
#include "model_loader.h"
#include "backend.h"
#include <iostream>

DrakonModel::DrakonModel(const std::string& model_path) {
    std::cout << "[🧠] DrakonModel constructor: loading model from " << model_path << std::endl;
    DrakonWeights weights = ModelLoader::load(model_path);
    backend_ = new DrakonBackend(weights.mock_weights);
}

float DrakonModel::forward(int token) {
    return backend_->forward(token);
}
