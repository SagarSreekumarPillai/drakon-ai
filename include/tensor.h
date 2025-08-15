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
    Tensor(const std::vector<size_t>& shape);         // shape-only with size_t
    Tensor(const std::vector<size_t>& shape, float val); // shape + fill with size_t

    float& operator()(int i, int j);
    float operator()(int i, int j) const;

    int rows() const;
    int cols() const;
    int size() const;
    int dims() const;  // Number of dimensions
    std::vector<int> getShape() const;
    std::vector<size_t> shape() const;  // Return shape as size_t
    void print(const std::string& label = "") const;

    std::vector<float>& data();         // raw access
    const std::vector<float>& data() const;

    // Arithmetic operators
    Tensor operator+(const Tensor& other) const;
    Tensor operator-(const Tensor& other) const;
    Tensor operator*(const Tensor& other) const;
    Tensor operator/(const Tensor& other) const;

private:
    std::vector<int> shape_; // Currently supports 2D: [rows, cols]
    std::vector<float> values_;
    int index(int i, int j) const;
};
