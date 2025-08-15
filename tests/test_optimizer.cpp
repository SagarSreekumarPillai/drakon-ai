#include "optimizer.h"
#include "layers.h"
#include "utils.h"
#include <iostream>
#include <cassert>

void test_sgd_optimizer() {
    std::cout << "Testing SGD optimizer..." << std::endl;
    
    // Create a simple layer
    LinearLayer layer(2, 1, "test_layer");
    auto params = layer.get_parameters();
    auto grads = layer.get_gradients();
    
    // Create SGD optimizer
    SGD sgd(0.01f, 0.9f);
    sgd.set_parameters(params, grads);
    
    // Store initial parameter values
    std::vector<float> initial_weights = params[0]->data();
    std::vector<float> initial_bias = params[1]->data();
    
    // Set some gradients
    for (float& grad : grads[0]->data()) {
        grad = 1.0f;
    }
    for (float& grad : grads[1]->data()) {
        grad = 0.5f;
    }
    
    // Take a step
    sgd.step();
    
    // Check that parameters were updated
    bool weights_changed = false;
    for (size_t i = 0; i < params[0]->size(); ++i) {
        if (std::abs(params[0]->data()[i] - initial_weights[i]) > 1e-6) {
            weights_changed = true;
            break;
        }
    }
    assert(weights_changed);
    
    std::cout << "✅ SGD optimizer tests passed" << std::endl;
}

void test_adam_optimizer() {
    std::cout << "Testing Adam optimizer..." << std::endl;
    
    // Create a simple layer
    LinearLayer layer(2, 1, "test_layer");
    auto params = layer.get_parameters();
    auto grads = layer.get_gradients();
    
    // Create Adam optimizer
    Adam adam(0.001f, 0.9f, 0.999f, 1e-8f);
    adam.set_parameters(params, grads);
    
    // Store initial parameter values
    std::vector<float> initial_weights = params[0]->data();
    
    // Take multiple steps to test momentum
    for (int step = 0; step < 5; ++step) {
        // Set gradients
        for (float& grad : grads[0]->data()) {
            grad = 1.0f;
        }
        for (float& grad : grads[1]->data()) {
            grad = 0.5f;
        }
        
        adam.step();
    }
    
    // Check that parameters were updated
    bool weights_changed = false;
    for (size_t i = 0; i < params[0]->size(); ++i) {
        if (std::abs(params[0]->data()[i] - initial_weights[i]) > 1e-6) {
            weights_changed = true;
            break;
        }
    }
    assert(weights_changed);
    
    std::cout << "✅ Adam optimizer tests passed" << std::endl;
}

void test_rmsprop_optimizer() {
    std::cout << "Testing RMSprop optimizer..." << std::endl;
    
    // Create a simple layer
    LinearLayer layer(2, 1, "test_layer");
    auto params = layer.get_parameters();
    auto grads = layer.get_gradients();
    
    // Create RMSprop optimizer
    RMSprop rmsprop(0.001f, 0.99f, 1e-8f);
    rmsprop.set_parameters(params, grads);
    
    // Store initial parameter values
    std::vector<float> initial_weights = params[0]->data();
    
    // Take multiple steps
    for (int step = 0; step < 5; ++step) {
        // Set gradients
        for (float& grad : grads[0]->data()) {
            grad = 1.0f;
        }
        for (float& grad : grads[1]->data()) {
            grad = 0.5f;
        }
        
        rmsprop.step();
    }
    
    // Check that parameters were updated
    bool weights_changed = false;
    for (size_t i = 0; i < params[0]->size(); ++i) {
        if (std::abs(params[0]->data()[i] - initial_weights[i]) > 1e-6) {
            weights_changed = true;
            break;
        }
    }
    assert(weights_changed);
    
    std::cout << "✅ RMSprop optimizer tests passed" << std::endl;
}

void test_optimizer_factory() {
    std::cout << "Testing optimizer factory..." << std::endl;
    
    // Test SGD creation
    auto sgd = create_optimizer("sgd", 0.01f);
    assert(sgd != nullptr);
    
    // Test Adam creation
    auto adam = create_optimizer("adam", 0.001f);
    assert(adam != nullptr);
    
    // Test RMSprop creation
    auto rmsprop = create_optimizer("rmsprop", 0.001f);
    assert(rmsprop != nullptr);
    
    // Test invalid optimizer type
    try {
        auto invalid = create_optimizer("invalid", 0.01f);
        assert(false);  // Should not reach here
    } catch (const std::invalid_argument&) {
        // Expected exception
    }
    
    std::cout << "✅ Optimizer factory tests passed" << std::endl;
}

void test_optimizer_parameter_updates() {
    std::cout << "Testing optimizer parameter updates..." << std::endl;
    
    // Create a more complex model
    MLP mlp({2, 4, 1}, "relu", "test_mlp");
    auto params = mlp.get_parameters();
    auto grads = mlp.get_gradients();
    
    // Create optimizer
    Adam optimizer(0.001f);
    optimizer.set_parameters(params, grads);
    
    // Store initial parameter values
    std::vector<std::vector<float>> initial_params;
    for (auto param : params) {
        initial_params.push_back(param->data());
    }
    
    // Simulate training steps
    for (int step = 0; step < 10; ++step) {
        // Set random gradients
        for (auto grad : grads) {
            for (float& g : grad->data()) {
                g = utils::random::random_float(-1.0f, 1.0f);
            }
        }
        
        optimizer.step();
        optimizer.zero_grad();
    }
    
    // Check that parameters were updated
    bool params_changed = false;
    for (size_t i = 0; i < params.size(); ++i) {
        for (size_t j = 0; j < params[i]->size(); ++j) {
            if (std::abs(params[i]->data()[j] - initial_params[i][j]) > 1e-6) {
                params_changed = true;
                break;
            }
        }
        if (params_changed) break;
    }
    assert(params_changed);
    
    std::cout << "✅ Optimizer parameter updates tests passed" << std::endl;
}

void test_learning_rate_management() {
    std::cout << "Testing learning rate management..." << std::endl;
    
    // Create optimizer
    Adam optimizer(0.001f);
    
    // Test get/set learning rate
    assert(std::abs(optimizer.get_learning_rate() - 0.001f) < 1e-6);
    
    optimizer.set_learning_rate(0.01f);
    assert(std::abs(optimizer.get_learning_rate() - 0.01f) < 1e-6);
    
    // Test with layer
    LinearLayer layer(2, 1, "test_layer");
    auto params = layer.get_parameters();
    auto grads = layer.get_gradients();
    
    optimizer.set_parameters(params, grads);
    
    // Set gradients and step
    for (float& grad : grads[0]->data()) {
        grad = 1.0f;
    }
    for (float& grad : grads[1]->data()) {
        grad = 0.5f;
    }
    
    optimizer.step();
    
    std::cout << "✅ Learning rate management tests passed" << std::endl;
}

int main() {
    std::cout << "=== Running Optimizer Tests ===" << std::endl;
    
    try {
        test_sgd_optimizer();
        test_adam_optimizer();
        test_rmsprop_optimizer();
        test_optimizer_factory();
        test_optimizer_parameter_updates();
        test_learning_rate_management();
        
        std::cout << "\n🎉 All optimizer tests passed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
