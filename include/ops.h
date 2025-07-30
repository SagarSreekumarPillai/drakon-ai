#pragma once
#include "tensor.h"

namespace ops {

Tensor matmul(const Tensor& A, const Tensor& B);
Tensor relu(const Tensor& A);
Tensor softmax(const Tensor& A);

}
