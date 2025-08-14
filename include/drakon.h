#ifndef DRAKON_H
#define DRAKON_H

#include <string>
#include <vector>
#include "model_loader.h"

class DrakonModel {
public:
    explicit DrakonModel(const std::string& model_path);

    // Forward pass: Dense + Softmax
    std::vector<float> forward(int token);

private:
    DrakonWeights weights;
};

#endif
