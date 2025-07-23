#include "layers/mlp.h"
#include "tensor_ops.h"

MLP::MLP(int input_size, int hidden_size, int output_size)
    : fc1(input_size, hidden_size), fc2(hidden_size, output_size) {}

Tensor MLP::forward(const Tensor& input) const {
    Tensor x = fc1.forward(input);
    x = TensorOps::gelu(x);
    return fc2.forward(x);
}
