#include "tensor_ops.h"
#include <stdexcept>
#include <cmath>
#include <iostream>

namespace TensorOps {

Tensor add(const Tensor& a, const Tensor& b) {
    if (a.rows() != b.rows() || a.cols() != b.cols()) {
        throw std::runtime_error("Shape mismatch in add()");
    }

    Tensor result(a.rows(), a.cols());
    for (int i = 0; i < a.size(); ++i) {
        result[i] = a[i] + b[i];
    }
    return result;
}

Tensor sub(const Tensor& a, const Tensor& b) {
    if (a.rows() != b.rows() || a.cols() != b.cols()) {
        throw std::runtime_error("Shape mismatch in sub()");
    }

    Tensor result(a.rows(), a.cols());
    for (int i = 0; i < a.size(); ++i) {
        result[i] = a[i] - b[i];
    }
    return result;
}

Tensor mul(const Tensor& a, const Tensor& b) {
    if (a.rows() != b.rows() || a.cols() != b.cols()) {
        throw std::runtime_error("Shape mismatch in mul()");
    }

    Tensor result(a.rows(), a.cols());
    for (int i = 0; i < a.size(); ++i) {
        result[i] = a[i] * b[i];
    }
    return result;
}

Tensor matmul(const Tensor& a, const Tensor& b) {
    if (a.cols() != b.rows()) {
        throw std::runtime_error("Shape mismatch in matmul()");
    }

    Tensor result(a.rows(), b.cols());
    for (int i = 0; i < a.rows(); ++i) {
        for (int j = 0; j < b.cols(); ++j) {
            float sum = 0.0f;
            for (int k = 0; k < a.cols(); ++k) {
                sum += a(i, k) * b(k, j);
            }
            result(i, j) = sum;
        }
    }
    return result;
}

Tensor relu(const Tensor& input) {
    Tensor result(input.rows(), input.cols());
    for (int i = 0; i < input.size(); ++i) {
        result[i] = std::max(0.0f, input[i]);
    }
    return result;
}

Tensor gelu(const Tensor& input) {
    Tensor result(input.rows(), input.cols());
    for (int i = 0; i < input.size(); ++i) {
        float x = input[i];
        result[i] = 0.5f * x * (1 + std::tanh(std::sqrt(2.0f / M_PI) * (x + 0.044715f * std::pow(x, 3))));
    }
    return result;
}

} // namespace TensorOps
