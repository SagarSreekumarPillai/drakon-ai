#include "tensor.h"
#include "tensor_ops.h"
#include "utils.h"
#include <iostream>
#include <cassert>

void test_tensor_creation() {
    std::cout << "Testing tensor creation..." << std::endl;
    
    // Test default constructor
    Tensor t1;
    assert(t1.size() == 1);
    assert(t1.dims() == 2);
    assert(t1.rows() == 1);
    assert(t1.cols() == 1);
    
    // Test shape constructor
    Tensor t2({2, 3});
    assert(t2.size() == 6);
    assert(t2.dims() == 2);
    assert(t2.shape()[0] == 2);
    assert(t2.shape()[1] == 3);
    
    // Test shape and value constructor
    Tensor t3({2, 2}, 1.5f);
    assert(t3.size() == 4);
    assert(t3(0, 0) == 1.5f);
    assert(t3(0, 1) == 1.5f);
    assert(t3(1, 0) == 1.5f);
    assert(t3(1, 1) == 1.5f);
    
    std::cout << "✅ Tensor creation tests passed" << std::endl;
}

void test_tensor_operations() {
    std::cout << "Testing tensor operations..." << std::endl;
    
    // Test element-wise operations
    Tensor a({2, 2}, 2.0f);
    Tensor b({2, 2}, 3.0f);
    
    // Addition
    Tensor c = a + b;
    assert(c(0, 0) == 5.0f);
    assert(c(0, 1) == 5.0f);
    assert(c(1, 0) == 5.0f);
    assert(c(1, 1) == 5.0f);
    
    // Subtraction
    Tensor d = b - a;
    assert(d(0, 0) == 1.0f);
    assert(d(0, 1) == 1.0f);
    assert(d(1, 0) == 1.0f);
    assert(d(1, 1) == 1.0f);
    
    // Multiplication
    Tensor e = a * b;
    assert(e(0, 0) == 6.0f);
    assert(e(0, 1) == 6.0f);
    assert(e(1, 0) == 6.0f);
    assert(e(1, 1) == 6.0f);
    
    // Division
    Tensor f = b / a;
    assert(f(0, 0) == 1.5f);
    assert(f(0, 1) == 1.5f);
    assert(f(1, 0) == 1.5f);
    assert(f(1, 1) == 1.5f);
    
    std::cout << "✅ Tensor operations tests passed" << std::endl;
}

void test_matrix_operations() {
    std::cout << "Testing matrix operations..." << std::endl;
    
    // Test matrix multiplication
    Tensor A({2, 3});
    A(0, 0) = 1; A(0, 1) = 2; A(0, 2) = 3;
    A(1, 0) = 4; A(1, 1) = 5; A(1, 2) = 6;
    
    Tensor B({3, 2});
    B(0, 0) = 7; B(0, 1) = 8;
    B(1, 0) = 9; B(1, 1) = 10;
    B(2, 0) = 11; B(2, 1) = 12;
    
    Tensor C = TensorOps::matmul(A, B);
    assert(C.rows() == 2);
    assert(C.cols() == 2);
    
    // Expected result:
    // [1*7 + 2*9 + 3*11, 1*8 + 2*10 + 3*12]
    // [4*7 + 5*9 + 6*11, 4*8 + 5*10 + 6*12]
    // = [58, 64]
    //   [139, 154]
    assert(std::abs(C(0, 0) - 58.0f) < 1e-6);
    assert(std::abs(C(0, 1) - 64.0f) < 1e-6);
    assert(std::abs(C(1, 0) - 139.0f) < 1e-6);
    assert(std::abs(C(1, 1) - 154.0f) < 1e-6);
    
    std::cout << "✅ Matrix operations tests passed" << std::endl;
}

void test_activation_functions() {
    std::cout << "Testing activation functions..." << std::endl;
    
    Tensor x({2, 2});
    x(0, 0) = -1.0f; x(0, 1) = 0.0f;
    x(1, 0) = 1.0f; x(1, 1) = 2.0f;
    
    // Test ReLU
    Tensor relu_result = TensorOps::relu(x);
    assert(relu_result(0, 0) == 0.0f);
    assert(relu_result(0, 1) == 0.0f);
    assert(relu_result(1, 0) == 1.0f);
    assert(relu_result(1, 1) == 2.0f);
    
    // Test GELU (since we don't have sigmoid implemented yet)
    Tensor gelu_result = TensorOps::gelu(x);
    // GELU behavior: negative values become negative, positive values stay positive
    assert(gelu_result(0, 0) < 0.0f);  // GELU(-1) < 0
    assert(std::abs(gelu_result(0, 1)) < 1e-6);  // GELU(0) ≈ 0
    assert(gelu_result(1, 0) > 0.0f);  // GELU(1) > 0
    assert(gelu_result(1, 1) > 0.0f);  // GELU(2) > 0
    
    std::cout << "✅ Activation functions tests passed" << std::endl;
}

void test_utility_functions() {
    std::cout << "Testing utility functions..." << std::endl;
    
    Tensor t({2, 3});
    t(0, 0) = 1.0f; t(0, 1) = 2.0f; t(0, 2) = 3.0f;
    t(1, 0) = 4.0f; t(1, 1) = 5.0f; t(1, 2) = 6.0f;
    
    // Test mean
    float mean_val = utils::math::mean(t);
    assert(std::abs(mean_val - 3.5f) < 1e-6);
    
    // Test sum
    float sum_val = utils::math::sum(t);
    assert(std::abs(sum_val - 21.0f) < 1e-6);
    
    // Test min/max
    float min_val = utils::math::min(t);
    float max_val = utils::math::max(t);
    assert(std::abs(min_val - 1.0f) < 1e-6);
    assert(std::abs(max_val - 6.0f) < 1e-6);
    
    std::cout << "✅ Utility functions tests passed" << std::endl;
}

void test_loss_functions() {
    std::cout << "Testing loss functions..." << std::endl;
    
    Tensor predictions({2, 2});
    predictions(0, 0) = 0.8f; predictions(0, 1) = 0.2f;
    predictions(1, 0) = 0.3f; predictions(1, 1) = 0.7f;
    
    Tensor targets({2, 2});
    targets(0, 0) = 1.0f; targets(0, 1) = 0.0f;
    targets(1, 0) = 0.0f; targets(1, 1) = 1.0f;
    
    // Test MSE loss
    float mse_loss = utils::loss::mse_loss(predictions, targets);
    assert(mse_loss > 0.0f);
    
    // Test cross-entropy loss
    float ce_loss = utils::loss::cross_entropy_loss(predictions, targets);
    assert(ce_loss > 0.0f);
    
    std::cout << "✅ Loss functions tests passed" << std::endl;
}

int main() {
    std::cout << "=== Running Tensor Tests ===" << std::endl;
    
    try {
        test_tensor_creation();
        test_tensor_operations();
        test_matrix_operations();
        test_activation_functions();
        test_utility_functions();
        test_loss_functions();
        
        std::cout << "\n🎉 All tensor tests passed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
