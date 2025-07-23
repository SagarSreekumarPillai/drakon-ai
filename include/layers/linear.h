#pragma once
#include "tensor.h"

class Linear {
public:
    Linear(int in_features, int out_features);
    Tensor forward(const Tensor& input) const;

private:
    Tensor weight;
    Tensor bias;
};
