#include "tensor.h"

Tensor::Tensor() : shape_({1, 1}) {
    values_.resize(1, 0.0f);
}

Tensor::Tensor(std::initializer_list<int> shape) {
    if (shape.size() != 2)
        throw std::invalid_argument("Only 2D tensors are supported for now.");
    shape_ = shape;
    if (shape_[0] <= 0 || shape_[1] <= 0) {
        throw std::invalid_argument("Tensor dimensions must be positive");
    }
    values_.resize(shape_[0] * shape_[1], 0.0f);
}

Tensor::Tensor(std::initializer_list<int> shape, float val) {
    if (shape.size() != 2)
        throw std::invalid_argument("Only 2D tensors are supported for now.");
    shape_ = shape;
    if (shape_[0] <= 0 || shape_[1] <= 0) {
        throw std::invalid_argument("Tensor dimensions must be positive");
    }
    values_.resize(shape_[0] * shape_[1], val);
}

Tensor::Tensor(const std::vector<size_t>& shape) {
    if (shape.size() != 2)
        throw std::invalid_argument("Only 2D tensors are supported for now.");
    shape_ = {static_cast<int>(shape[0]), static_cast<int>(shape[1])};
    if (shape_[0] <= 0 || shape_[1] <= 0) {
        throw std::invalid_argument("Tensor dimensions must be positive");
    }
    values_.resize(shape_[0] * shape_[1], 0.0f);
}

Tensor::Tensor(const std::vector<size_t>& shape, float val) {
    if (shape.size() != 2)
        throw std::invalid_argument("Only 2D tensors are supported for now.");
    shape_ = {static_cast<int>(shape[0]), static_cast<int>(shape[1])};
    if (shape_[0] <= 0 || shape_[1] <= 0) {
        throw std::invalid_argument("Tensor dimensions must be positive");
    }
    values_.resize(shape_[0] * shape_[1], val);
}

int Tensor::index(int i, int j) const {
    if (shape_.empty() || shape_[0] == 0 || shape_[1] == 0) {
        throw std::runtime_error("Cannot access tensor with zero dimensions");
    }
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

int Tensor::dims() const {
    return static_cast<int>(shape_.size());
}

std::vector<int> Tensor::getShape() const {
    return shape_;
}

std::vector<size_t> Tensor::shape() const {
    return {static_cast<size_t>(shape_[0]), static_cast<size_t>(shape_[1])};
}

std::vector<float>& Tensor::data() {
    return values_;
}

const std::vector<float>& Tensor::data() const {
    return values_;
}

// Arithmetic operators
Tensor Tensor::operator+(const Tensor& other) const {
    if (shape_ != other.shape_) {
        throw std::invalid_argument("Tensor shapes must match for addition");
    }
    
    std::vector<size_t> shape_vec = {static_cast<size_t>(shape_[0]), static_cast<size_t>(shape_[1])};
    Tensor result(shape_vec);
    for (size_t i = 0; i < values_.size(); ++i) {
        result.values_[i] = values_[i] + other.values_[i];
    }
    return result;
}

Tensor Tensor::operator-(const Tensor& other) const {
    if (shape_ != other.shape_) {
        throw std::invalid_argument("Tensor shapes must match for subtraction");
    }
    
    std::vector<size_t> shape_vec = {static_cast<size_t>(shape_[0]), static_cast<size_t>(shape_[1])};
    Tensor result(shape_vec);
    for (size_t i = 0; i < values_.size(); ++i) {
        result.values_[i] = values_[i] - other.values_[i];
    }
    return result;
}

Tensor Tensor::operator*(const Tensor& other) const {
    if (shape_ != other.shape_) {
        throw std::invalid_argument("Tensor shapes must match for multiplication");
    }
    
    std::vector<size_t> shape_vec = {static_cast<size_t>(shape_[0]), static_cast<size_t>(shape_[1])};
    Tensor result(shape_vec);
    for (size_t i = 0; i < values_.size(); ++i) {
        result.values_[i] = values_[i] * other.values_[i];
    }
    return result;
}

Tensor Tensor::operator/(const Tensor& other) const {
    if (shape_ != other.shape_) {
        throw std::invalid_argument("Tensor shapes must match for division");
    }
    
    std::vector<size_t> shape_vec = {static_cast<size_t>(shape_[0]), static_cast<size_t>(shape_[1])};
    Tensor result(shape_vec);
    for (size_t i = 0; i < values_.size(); ++i) {
        if (other.values_[i] == 0.0f) {
            throw std::invalid_argument("Division by zero");
        }
        result.values_[i] = values_[i] / other.values_[i];
    }
    return result;
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
