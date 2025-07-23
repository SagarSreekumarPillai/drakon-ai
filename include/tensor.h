#pragma once
#include <vector>
#include <iostream>
#include <stdexcept>
#include <initializer_list>

class Tensor {
public:
    Tensor();  // Default
    Tensor(std::initializer_list<int> shape);         // shape-only
    Tensor(std::initializer_list<int> shape, float val); // shape + fill

    float& operator()(int i, int j);
    float operator()(int i, int j) const;

    int rows() const;
    int cols() const;
    int size() const;
    std::vector<int> getShape() const;
    void print(const std::string& label = "") const;

    std::vector<float>& data();         // raw access
    const std::vector<float>& data() const;

private:
    std::vector<int> shape_; // Currently supports 2D: [rows, cols]
    std::vector<float> values_;
    int index(int i, int j) const;
};
