#pragma once
#include <string>

class DrakonModel {
public:
    DrakonModel(const std::string& model_path);
    float forward(int token);  // simplified placeholder

private:
    // Add internal buffers later
};
