#include "model_loader.h"
#include "tokenizer.h"
#include "backend.h"
#include "drakon.h"
#include <iostream>

DrakonModel::DrakonModel(const std::string& model_path) {
    std::cout << "[🧠] DrakonModel constructor: loading model from " << model_path << std::endl;
    // TODO: Connect to model_loader
}

float DrakonModel::forward(int token) {
    // TODO: Replace with actual model logic
    return token * 0.01f;
}
