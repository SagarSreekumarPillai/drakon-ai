#pragma once
#include "tensor.h"

namespace TensorOps {

Tensor add(const Tensor& a, const Tensor& b);
Tensor sub(const Tensor& a, const Tensor& b);
Tensor mul(const Tensor& a, const Tensor& b);
Tensor matmul(const Tensor& a, const Tensor& b);

Tensor relu(const Tensor& input);
Tensor gelu(const Tensor& input);  // Approximate version
}
