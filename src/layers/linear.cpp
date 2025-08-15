#include "layers/linear.h"
#include "tensor_ops.h"
#include <random>

Linear::Linear(int in_features, int out_features)
    : weight({out_features, in_features}), bias({out_features, 1}) {

    std::default_random_engine gen(42);
    std::normal_distribution<float> dist(0.0, 0.02);

    for (int i = 0; i < weight.size(); ++i) {
        weight.data()[i] = dist(gen);
    }
    for (int i = 0; i < bias.size(); ++i) {
        bias.data()[i] = 0.0f;
    }
}

Tensor Linear::forward(const Tensor& input) const {
    // Assume input is (in_features, 1)
    // Output = W x + b
    Tensor out = TensorOps::matmul(weight, input);  // (out_features, 1)
    return TensorOps::add(out, bias);
}
