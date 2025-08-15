#include "layers.h"
#include "utils.h"
#include <iostream>
#include <cassert>

void test_linear_layer() {
    std::cout << "Testing linear layer..." << std::endl;
    
    // Create a linear layer
    LinearLayer layer(3, 2, "test_linear");
    
    // Test parameter count
    assert(layer.num_parameters() == 8);  // 3*2 weights + 2 bias
    
    // Test forward pass
    Tensor input({2, 3});
    input(0, 0) = 1.0f; input(0, 1) = 2.0f; input(0, 2) = 3.0f;
    input(1, 0) = 4.0f; input(1, 1) = 5.0f; input(1, 2) = 6.0f;
    
    Tensor output = layer.forward(input);
    assert(output.rows() == 2);
    assert(output.cols() == 2);
    
    // Test backward pass (simplified implementation)
    Tensor grad_output({2, 2}, 1.0f);
    Tensor grad_input = layer.backward(grad_output);
    assert(grad_input.rows() == 2);
    assert(grad_input.cols() == 3);  // Should return input shape (2x3)
    
    // Test parameter access
    auto params = layer.get_parameters();
    assert(params.size() == 2);  // weights and bias
    
    auto grads = layer.get_gradients();
    assert(grads.size() == 2);  // weight gradients and bias gradients
    
    std::cout << "✅ Linear layer tests passed" << std::endl;
}

void test_mlp() {
    std::cout << "Testing MLP..." << std::endl;
    
    // Create a simple MLP first to test basic functionality
    std::vector<size_t> simple_layer_sizes = {2, 3, 1};
    MLP simple_mlp(simple_layer_sizes, "relu", "simple_test_mlp");
    
    // Test parameter count for simple MLP
    size_t simple_expected_params = 2*3 + 3 + 3*1 + 1;  // weights + biases
    assert(simple_mlp.num_parameters() == simple_expected_params);
    
    // Test forward pass for simple MLP
    Tensor simple_input({1, 2});
    simple_input(0, 0) = 1.0f;
    simple_input(0, 1) = 2.0f;
    
    Tensor simple_output = simple_mlp.forward(simple_input);
    assert(simple_output.rows() == 1);
    assert(simple_output.cols() == 1);
    
    std::cout << "✅ Simple MLP tests passed" << std::endl;
    
    // Now test a medium-sized MLP
    std::vector<size_t> medium_layer_sizes = {3, 4, 2};
    MLP medium_mlp(medium_layer_sizes, "relu", "medium_test_mlp");
    
    // Test parameter count for medium MLP
    size_t medium_expected_params = 3*4 + 4 + 4*2 + 2;  // weights + biases
    assert(medium_mlp.num_parameters() == medium_expected_params);
    
    // Test forward pass for medium MLP
    Tensor medium_input({2, 3});
    medium_input(0, 0) = 1.0f; medium_input(0, 1) = 2.0f; medium_input(0, 2) = 3.0f;
    medium_input(1, 0) = 4.0f; medium_input(1, 1) = 5.0f; medium_input(1, 2) = 6.0f;
    
    Tensor medium_output = medium_mlp.forward(medium_input);
    assert(medium_output.rows() == 2);
    assert(medium_output.cols() == 2);
    
    // Test backward pass for medium MLP
    Tensor medium_grad_output({2, 2}, 1.0f);
    Tensor medium_grad_input = medium_mlp.backward(medium_grad_output);
    assert(medium_grad_input.rows() == 2);
    assert(medium_grad_input.cols() == 3);  // Should return input shape (2x3)
    
    std::cout << "✅ Medium MLP tests passed" << std::endl;
    
    // Now test the original MLP
    std::vector<size_t> layer_sizes = {4, 8, 6, 2};
    MLP mlp(layer_sizes, "relu", "test_mlp");
    
    // Test parameter count
    size_t expected_params = 4*8 + 8 + 8*6 + 6 + 6*2 + 2;  // weights + biases
    assert(mlp.num_parameters() == expected_params);
    
    // Test forward pass
    Tensor input({3, 4});
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            input(i, j) = static_cast<float>(i + j);
        }
    }
    
    Tensor output = mlp.forward(input);
    assert(output.rows() == 3);
    assert(output.cols() == 2);
    
    // Test backward pass (simplified implementation)
    Tensor grad_output({3, 2}, 1.0f);
    Tensor grad_input = mlp.backward(grad_output);
    assert(grad_input.rows() == 3);
    assert(grad_input.cols() == 4);  // Should return input shape (3x4)
    
    // Test parameter access
    auto params = mlp.get_parameters();
    assert(params.size() == 6);  // 3 layers * 2 parameters each
    
    auto grads = mlp.get_gradients();
    assert(grads.size() == 6);  // 3 layers * 2 gradients each
    
    std::cout << "✅ MLP tests passed" << std::endl;
}

void test_dropout_layer() {
    std::cout << "Testing dropout layer..." << std::endl;
    
    // Create a dropout layer
    DropoutLayer dropout(0.5f, "test_dropout");
    
    // Test parameter count (should be 0)
    assert(dropout.num_parameters() == 0);
    
    // Test forward pass in training mode
    Tensor input({2, 3}, 1.0f);
    Tensor output = dropout.forward(input);
    
    // In training mode, some values should be zeroed out
    bool has_zeros = false;
    for (const float& val : output.data()) {
        if (val == 0.0f) {
            has_zeros = true;
            break;
        }
    }
    assert(has_zeros);
    
    // Test backward pass
    Tensor grad_output({2, 3}, 1.0f);
    Tensor grad_input = dropout.backward(grad_output);
    assert(grad_input.rows() == 2);
    assert(grad_input.cols() == 3);
    
    // Test evaluation mode
    dropout.set_training(false);
    Tensor eval_output = dropout.forward(input);
    
    // In evaluation mode, output should equal input
    for (size_t i = 0; i < input.size(); ++i) {
        assert(std::abs(eval_output.data()[i] - input.data()[i]) < 1e-6);
    }
    
    std::cout << "✅ Dropout layer tests passed" << std::endl;
}

void test_layer_forward_backward() {
    std::cout << "Testing layer forward/backward consistency..." << std::endl;
    
    // Create a simple network: Linear -> ReLU -> Linear
    LinearLayer layer1(2, 3, "layer1");
    LinearLayer layer2(3, 1, "layer2");
    
    // Test forward pass
    Tensor input({1, 2});
    input(0, 0) = 1.0f;
    input(0, 1) = 2.0f;
    
    Tensor hidden = layer1.forward(input);
    hidden = activation::relu(hidden);
    Tensor output = layer2.forward(hidden);
    
    assert(output.rows() == 1);
    assert(output.cols() == 1);
    
    // Test backward pass
    Tensor grad_output({1, 1}, 1.0f);
    Tensor grad_hidden = layer2.backward(grad_output);
    grad_hidden = grad_hidden * activation::relu_derivative(hidden);
    Tensor grad_input = layer1.backward(grad_hidden);
    
    assert(grad_input.rows() == 1);
    assert(grad_input.cols() == 2);
    
    std::cout << "✅ Layer forward/backward consistency tests passed" << std::endl;
}

void test_parameter_gradients() {
    std::cout << "Testing parameter gradients..." << std::endl;
    
    LinearLayer layer(2, 1, "test_layer");
    
    // Get initial parameters
    auto params = layer.get_parameters();
    auto grads = layer.get_gradients();
    
    // Store initial parameter values
    std::vector<float> initial_weights = params[0]->data();
    std::vector<float> initial_bias = params[1]->data();
    
    // Forward pass
    Tensor input({1, 2}, 1.0f);
    Tensor output = layer.forward(input);
    
    // Backward pass
    Tensor grad_output({1, 1}, 1.0f);
    layer.backward(grad_output);
    
    // Check that gradients are computed
    bool has_nonzero_grad = false;
    for (const float& grad : grads[0]->data()) {
        if (std::abs(grad) > 1e-6) {
            has_nonzero_grad = true;
            break;
        }
    }
    assert(has_nonzero_grad);
    
    std::cout << "✅ Parameter gradients tests passed" << std::endl;
}

int main() {
    std::cout << "=== Running Layer Tests ===" << std::endl;
    
    try {
        test_linear_layer();
        test_mlp();
        test_dropout_layer();
        test_layer_forward_backward();
        test_parameter_gradients();
        
        std::cout << "\n🎉 All layer tests passed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
