#include "tensor.h"

Tensor::Tensor() : shape_({0, 0}) {}

Tensor::Tensor(std::initializer_list<int> shape) {
    if (shape.size() != 2)
        throw std::invalid_argument("Only 2D tensors are supported for now.");
    shape_ = shape;
    values_.resize(shape_[0] * shape_[1], 0.0f);
}

Tensor::Tensor(std::initializer_list<int> shape, float val) {
    if (shape.size() != 2)
        throw std::invalid_argument("Only 2D tensors are supported for now.");
    shape_ = shape;
    values_.resize(shape_[0] * shape_[1], val);
}

int Tensor::index(int i, int j) const {
    return i * shape_[1] + j;
}

float& Tensor::operator()(int i, int j) {
    return values_.at(index(i, j));
}

float Tensor::operator()(int i, int j) const {
    return values_.at(index(i, j));
}

int Tensor::rows() const {
    return shape_[0];
}

int Tensor::cols() const {
    return shape_[1];
}

int Tensor::size() const {
    return static_cast<int>(values_.size());
}

std::vector<int> Tensor::getShape() const {
    return shape_;
}

std::vector<float>& Tensor::data() {
    return values_;
}

const std::vector<float>& Tensor::data() const {
    return values_;
}

void Tensor::print(const std::string& label) const {
    if (!label.empty()) std::cout << label << ":\n";

    for (int i = 0; i < shape_[0]; ++i) {
        for (int j = 0; j < shape_[1]; ++j) {
            std::cout << (*this)(i, j) << " ";
        }
        std::cout << "\n";
    }
}
