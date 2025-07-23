#pragma once
#include "layers/linear.h"

class MLP {
public:
    MLP(int input_size, int hidden_size, int output_size);
    Tensor forward(const Tensor& input) const;

private:
    Linear fc1;
    Linear fc2;
};
