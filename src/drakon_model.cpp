#include "drakon.h"
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>

static std::vector<float> softmax(const std::vector<float>& logits) {
    std::vector<float> exps(logits.size());
    float max_logit = *std::max_element(logits.begin(), logits.end());
    float sum = 0.0f;
    for (size_t i = 0; i < logits.size(); i++) {
        exps[i] = std::exp(logits[i] - max_logit); // stability trick
        sum += exps[i];
    }
    for (size_t i = 0; i < logits.size(); i++) {
        exps[i] /= sum;
    }
    return exps;
}

DrakonModel::DrakonModel(const std::string& model_path) {
    std::cout << "[🧠] Initializing DrakonModel with path: " << model_path << std::endl;
    ModelLoader loader;
    weights = loader.load(model_path);
}

std::vector<float> DrakonModel::forward(int token) {
    // Dense layer: multiply token scalar by each weight
    std::vector<float> logits;
    logits.reserve(weights.mock_weights.size());
    for (float w : weights.mock_weights) {
        logits.push_back(w * token);
    }

    // Softmax layer
    return softmax(logits);
}
