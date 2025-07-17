// src/backend.cpp

#include "backend.h"
#include <random>
#include <iostream>

DrakonBackend::DrakonBackend(const std::vector<float>& weights)
    : weights_(weights) {}

float DrakonBackend::forward(int token) {
    // Mock logic: generate a random float to simulate inference
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(10.0, 60.0);

    float mock_result = dis(gen);
    std::cout << "[📤] Predicted value (mock): " << mock_result << std::endl;

    return mock_result;
}
