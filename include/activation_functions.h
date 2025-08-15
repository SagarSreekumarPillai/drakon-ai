#ifndef ACTIVATION_FUNCTIONS_H
#define ACTIVATION_FUNCTIONS_H

#include "tensor.h"
#include <cmath>

namespace activation {

/**
 * @brief ReLU activation function: max(0, x)
 * @param x Input tensor
 * @return Tensor with ReLU applied element-wise
 */
Tensor relu(const Tensor& x);

/**
 * @brief Sigmoid activation function: 1 / (1 + e^(-x))
 * @param x Input tensor
 * @return Tensor with sigmoid applied element-wise
 */
Tensor sigmoid(const Tensor& x);

/**
 * @brief Tanh activation function: (e^x - e^(-x)) / (e^x + e^(-x))
 * @param x Input tensor
 * @return Tensor with tanh applied element-wise
 */
Tensor tanh(const Tensor& x);

/**
 * @brief Softmax activation function (applied along the last dimension)
 * @param x Input tensor
 * @return Tensor with softmax applied
 */
Tensor softmax(const Tensor& x);

/**
 * @brief Derivative of ReLU: 1 if x > 0, 0 otherwise
 * @param x Input tensor
 * @return Tensor with ReLU derivative applied element-wise
 */
Tensor relu_derivative(const Tensor& x);

/**
 * @brief Derivative of sigmoid: sigmoid(x) * (1 - sigmoid(x))
 * @param x Input tensor
 * @return Tensor with sigmoid derivative applied element-wise
 */
Tensor sigmoid_derivative(const Tensor& x);

/**
 * @brief Derivative of tanh: 1 - tanh(x)^2
 * @param x Input tensor
 * @return Tensor with tanh derivative applied element-wise
 */
Tensor tanh_derivative(const Tensor& x);

} // namespace activation

#endif // ACTIVATION_FUNCTIONS_H
