#include "ops.h"
#include <cmath>
#include <stdexcept>

namespace ops {

// Matrix Multiplication: C = A x B
Tensor matmul(const Tensor& A, const Tensor& B) {
    if (A.cols() != B.rows())
        throw std::invalid_argument("Incompatible shapes for matmul");

    Tensor result({A.rows(), B.cols()}, 0.0f);

    for (int i = 0; i < A.rows(); ++i) {
        for (int j = 0; j < B.cols(); ++j) {
            float sum = 0.0f;
            for (int k = 0; k < A.cols(); ++k) {
                sum += A(i, k) * B(k, j);
            }
            result(i, j) = sum;
        }
    }
    return result;
}

// ReLU: max(0, x)
Tensor relu(const Tensor& A) {
    Tensor out = A;
    for (float& v : out.data()) {
        v = std::max(0.0f, v);
    }
    return out;
}

// Row-wise Softmax
Tensor softmax(const Tensor& A) {
    Tensor out({A.rows(), A.cols()});
    for (int i = 0; i < A.rows(); ++i) {
        float max_val = -std::numeric_limits<float>::infinity();
        for (int j = 0; j < A.cols(); ++j)
            max_val = std::max(max_val, A(i, j));

        float sum_exp = 0.0f;
        for (int j = 0; j < A.cols(); ++j)
            sum_exp += std::exp(A(i, j) - max_val);

        for (int j = 0; j < A.cols(); ++j)
            out(i, j) = std::exp(A(i, j) - max_val) / sum_exp;
    }
    return out;
}

}
