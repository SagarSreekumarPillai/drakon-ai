// drakon-ai/src/drakon_model.cpp
#include "drakon.h"
#include "model_loader.h"
#include <iostream>
#include <numeric>   // std::accumulate

// Helper: compute deterministic “score” from weights
static inline float score_from_weights(const std::vector<float>& w) {
    // Sum as a simple stand-in for a real forward pass
    return std::accumulate(w.begin(), w.end(), 0.0f);
}

DrakonModel::DrakonModel(const std::string& model_path) {
    std::cout << "[🧠] Initializing DrakonModel with path: " << model_path << std::endl;
    ModelLoader loader;
    weights = loader.load(model_path);
}

float DrakonModel::forward(int token) {
    if (use_cache_) {
        auto it = cache_.find(token);
        if (it != cache_.end()) return it->second;
    }

    // Dummy computation: (sum(weights) * token)
    float s = score_from_weights(weights.mock_weights);
    float out = s * static_cast<float>(token);

    if (use_cache_) cache_[token] = out;
    return out;
}

std::vector<float> DrakonModel::forward_batch(const std::vector<int>& tokens) {
    std::vector<float> outputs;
    outputs.reserve(tokens.size());

    // Precompute “score”
    float s = score_from_weights(weights.mock_weights);

    for (int t : tokens) {
        if (use_cache_) {
            auto it = cache_.find(t);
            if (it != cache_.end()) {
                outputs.push_back(it->second);
                continue;
            }
        }
        float out = s * static_cast<float>(t);
        if (use_cache_) cache_[t] = out;
        outputs.push_back(out);
    }
    return outputs;
}
