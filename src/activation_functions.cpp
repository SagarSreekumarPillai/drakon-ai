#include "activation_functions.h"
#include "tensor_ops.h"
#include <cmath>
#include <algorithm>

namespace activation {

Tensor relu(const Tensor& x) {
    Tensor result = x;
    for (float& val : result.data()) {
        val = std::max(0.0f, val);
    }
    return result;
}

Tensor sigmoid(const Tensor& x) {
    Tensor result = x;
    for (float& val : result.data()) {
        val = 1.0f / (1.0f + std::exp(-val));
    }
    return result;
}

Tensor tanh(const Tensor& x) {
    Tensor result = x;
    for (float& val : result.data()) {
        val = std::tanh(val);
    }
    return result;
}

Tensor softmax(const Tensor& x) {
    // For simplicity, assume 2D tensor (batch_size, num_classes)
    if (x.dims() != 2) {
        throw std::invalid_argument("Softmax expects 2D tensor");
    }
    
    Tensor result = x;
    size_t batch_size = x.shape()[0];
    size_t num_classes = x.shape()[1];
    
    for (size_t i = 0; i < batch_size; ++i) {
        // Find max for numerical stability
        float max_val = -std::numeric_limits<float>::infinity();
        for (size_t j = 0; j < num_classes; ++j) {
            max_val = std::max(max_val, x(i, j));
        }
        
        // Compute exp(x - max) and sum
        float sum_exp = 0.0f;
        for (size_t j = 0; j < num_classes; ++j) {
            float exp_val = std::exp(x(i, j) - max_val);
            result(i, j) = exp_val;
            sum_exp += exp_val;
        }
        
        // Normalize
        for (size_t j = 0; j < num_classes; ++j) {
            result(i, j) /= sum_exp;
        }
    }
    
    return result;
}

Tensor relu_derivative(const Tensor& x) {
    Tensor result = x;
    for (float& val : result.data()) {
        val = (val > 0.0f) ? 1.0f : 0.0f;
    }
    return result;
}

Tensor sigmoid_derivative(const Tensor& x) {
    Tensor sig = sigmoid(x);
    Tensor result = sig;
    for (size_t i = 0; i < sig.size(); ++i) {
        result.data()[i] = sig.data()[i] * (1.0f - sig.data()[i]);
    }
    return result;
}

Tensor tanh_derivative(const Tensor& x) {
    Tensor tanh_val = tanh(x);
    Tensor result = tanh_val;
    for (size_t i = 0; i < tanh_val.size(); ++i) {
        result.data()[i] = 1.0f - tanh_val.data()[i] * tanh_val.data()[i];
    }
    return result;
}

} // namespace activation
