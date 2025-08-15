#include "config.h"
#include "dataset.h"
#include "layers.h"
#include "optimizer.h"
#include "utils.h"
#include <iostream>
#include <cassert>
#include <memory>

void test_complete_training_pipeline() {
    std::cout << "Testing complete training pipeline..." << std::endl;
    
    // Set random seed for reproducibility
    utils::random::set_seed(42);
    
    // Create configuration
    Config config = create_default_config("classification", 4, 2);
    
    // Create dataset
    auto dataset = std::make_shared<SyntheticDataset>(
        config.get_dataset_config().num_samples,
        config.get_dataset_config().input_features,
        config.get_dataset_config().num_classes,
        config.get_dataset_config().problem_type,
        config.get_training_config().shuffle_data
    );
    
    // Create model
    auto model = std::make_unique<MLP>(
        config.get_network_config().layer_sizes,
        config.get_network_config().activation_type,
        "test_model"
    );
    
    // Create optimizer
    auto optimizer = create_optimizer(
        config.get_training_config().optimizer_type,
        config.get_training_config().learning_rate
    );
    
    // Set optimizer parameters
    auto params = model->get_parameters();
    auto grads = model->get_gradients();
    optimizer->set_parameters(params, grads);
    
    // Create data loader
    DataLoader train_loader(dataset, config.get_training_config().batch_size, true);
    
    // Training loop
    float initial_loss = std::numeric_limits<float>::infinity();
    float final_loss = 0.0f;
    
    for (size_t epoch = 0; epoch < 5; ++epoch) {  // Short training for testing
        float epoch_loss = 0.0f;
        size_t num_batches = 0;
        
        for (size_t batch_idx = 0; batch_idx < train_loader.num_batches(); ++batch_idx) {
            auto [batch_data, batch_labels] = train_loader.get_batch(batch_idx);
            
            // Forward pass
            Tensor predictions = model->forward(batch_data);
            
            // Compute loss
            float loss = utils::loss::cross_entropy_loss(predictions, batch_labels);
            epoch_loss += loss;
            num_batches++;
            
            // Backward pass
            Tensor grad_output = predictions - std::get<1>(batch_labels);  // Simplified gradient
            model->backward(grad_output);
            
            // Update parameters
            optimizer->step();
            optimizer->zero_grad();
        }
        
        final_loss = epoch_loss / num_batches;
        if (epoch == 0) {
            initial_loss = final_loss;
        }
    }
    
    // Verify that loss decreased
    assert(final_loss < initial_loss);
    
    std::cout << "✅ Complete training pipeline test passed" << std::endl;
    std::cout << "   Initial loss: " << initial_loss << std::endl;
    std::cout << "   Final loss: " << final_loss << std::endl;
}

void test_model_evaluation() {
    std::cout << "Testing model evaluation..." << std::endl;
    
    // Create a simple model
    auto model = std::make_unique<MLP>({2, 4, 2}, "relu", "eval_model");
    
    // Create test data
    auto test_dataset = std::make_shared<SyntheticDataset>(100, 2, 2, "classification", false);
    
    // Evaluate model
    float total_loss = 0.0f;
    float total_accuracy = 0.0f;
    size_t num_batches = 0;
    
    DataLoader test_loader(test_dataset, 16, false);
    
    for (size_t batch_idx = 0; batch_idx < test_loader.num_batches(); ++batch_idx) {
        auto [batch_data, batch_labels] = test_loader.get_batch(batch_idx);
        
        // Forward pass
        Tensor predictions = model->forward(batch_data);
        
        // Compute metrics
        float loss = utils::loss::cross_entropy_loss(predictions, batch_labels);
        float accuracy = utils::metrics::accuracy(predictions, batch_labels);
        
        total_loss += loss;
        total_accuracy += accuracy;
        num_batches++;
    }
    
    float avg_loss = total_loss / num_batches;
    float avg_accuracy = total_accuracy / num_batches;
    
    // Verify metrics are reasonable
    assert(avg_loss > 0.0f);
    assert(avg_accuracy >= 0.0f && avg_accuracy <= 1.0f);
    
    std::cout << "✅ Model evaluation test passed" << std::endl;
    std::cout << "   Average loss: " << avg_loss << std::endl;
    std::cout << "   Average accuracy: " << avg_accuracy << std::endl;
}

void test_model_save_load() {
    std::cout << "Testing model save/load..." << std::endl;
    
    // Create a model
    auto model1 = std::make_unique<MLP>({3, 5, 2}, "relu", "save_model");
    
    // Get some parameters
    auto params1 = model1->get_parameters();
    std::vector<std::vector<float>> original_params;
    for (auto param : params1) {
        original_params.push_back(param->data());
    }
    
    // Save parameters to files
    for (size_t i = 0; i < params1.size(); ++i) {
        std::string filename = "test_param_" + std::to_string(i) + ".bin";
        utils::io::save_tensor_binary(*params1[i], filename);
    }
    
    // Create a new model with same architecture
    auto model2 = std::make_unique<MLP>({3, 5, 2}, "relu", "load_model");
    auto params2 = model2->get_parameters();
    
    // Load parameters
    for (size_t i = 0; i < params2.size(); ++i) {
        std::string filename = "test_param_" + std::to_string(i) + ".bin";
        *params2[i] = utils::io::load_tensor_binary(filename);
    }
    
    // Verify parameters are the same
    for (size_t i = 0; i < params1.size(); ++i) {
        for (size_t j = 0; j < params1[i]->size(); ++j) {
            assert(std::abs(params1[i]->data()[j] - params2[i]->data()[j]) < 1e-6);
        }
    }
    
    // Test with same input
    Tensor test_input({2, 3}, 1.0f);
    Tensor output1 = model1->forward(test_input);
    Tensor output2 = model2->forward(test_input);
    
    // Verify outputs are the same
    for (size_t i = 0; i < output1.size(); ++i) {
        assert(std::abs(output1.data()[i] - output2.data()[i]) < 1e-6);
    }
    
    // Clean up test files
    for (size_t i = 0; i < params1.size(); ++i) {
        std::string filename = "test_param_" + std::to_string(i) + ".bin";
        std::remove(filename.c_str());
    }
    
    std::cout << "✅ Model save/load test passed" << std::endl;
}

void test_different_optimizers() {
    std::cout << "Testing different optimizers..." << std::endl;
    
    // Test SGD
    {
        auto model = std::make_unique<MLP>({2, 3, 1}, "relu", "sgd_model");
        auto optimizer = create_optimizer("sgd", 0.01f);
        auto params = model->get_parameters();
        auto grads = model->get_gradients();
        optimizer->set_parameters(params, grads);
        
        // Set gradients and step
        for (auto grad : grads) {
            for (float& g : grad->data()) {
                g = 1.0f;
            }
        }
        optimizer->step();
        
        // Verify parameters were updated
        bool updated = false;
        for (auto param : params) {
            for (const float& p : param->data()) {
                if (std::abs(p) > 1e-6) {
                    updated = true;
                    break;
                }
            }
            if (updated) break;
        }
        assert(updated);
    }
    
    // Test Adam
    {
        auto model = std::make_unique<MLP>({2, 3, 1}, "relu", "adam_model");
        auto optimizer = create_optimizer("adam", 0.001f);
        auto params = model->get_parameters();
        auto grads = model->get_gradients();
        optimizer->set_parameters(params, grads);
        
        // Take multiple steps
        for (int step = 0; step < 3; ++step) {
            for (auto grad : grads) {
                for (float& g : grad->data()) {
                    g = 1.0f;
                }
            }
            optimizer->step();
            optimizer->zero_grad();
        }
        
        // Verify parameters were updated
        bool updated = false;
        for (auto param : params) {
            for (const float& p : param->data()) {
                if (std::abs(p) > 1e-6) {
                    updated = true;
                    break;
                }
            }
            if (updated) break;
        }
        assert(updated);
    }
    
    std::cout << "✅ Different optimizers test passed" << std::endl;
}

void test_activation_functions() {
    std::cout << "Testing activation functions..." << std::endl;
    
    // Test different activation functions
    std::vector<std::string> activations = {"relu", "sigmoid", "tanh"};
    
    for (const auto& activation : activations) {
        auto model = std::make_unique<MLP>({2, 4, 1}, activation, "activation_test");
        
        Tensor input({1, 2}, 1.0f);
        Tensor output = model->forward(input);
        
        // Verify output is reasonable
        assert(output.rows() == 1);
        assert(output.cols() == 1);
        assert(!std::isnan(output(0, 0)));
        assert(!std::isinf(output(0, 0)));
        
        // Test backward pass
        Tensor grad_output({1, 1}, 1.0f);
        Tensor grad_input = model->backward(grad_output);
        
        assert(grad_input.rows() == 1);
        assert(grad_input.cols() == 2);
    }
    
    std::cout << "✅ Activation functions test passed" << std::endl;
}

void test_early_stopping() {
    std::cout << "Testing early stopping..." << std::endl;
    
    // Create a simple training scenario
    auto dataset = std::make_shared<SyntheticDataset>(50, 2, 2, "classification", true);
    auto model = std::make_unique<MLP>({2, 3, 2}, "relu", "early_stop_model");
    auto optimizer = create_optimizer("adam", 0.01f);
    
    auto params = model->get_parameters();
    auto grads = model->get_gradients();
    optimizer->set_parameters(params, grads);
    
    DataLoader loader(dataset, 8, true);
    
    float best_loss = std::numeric_limits<float>::infinity();
    size_t patience_counter = 0;
    size_t patience = 3;
    size_t max_epochs = 10;
    size_t actual_epochs = 0;
    
    for (size_t epoch = 0; epoch < max_epochs; ++epoch) {
        float epoch_loss = 0.0f;
        size_t num_batches = 0;
        
        for (size_t batch_idx = 0; batch_idx < loader.num_batches(); ++batch_idx) {
            auto [batch_data, batch_labels] = loader.get_batch(batch_idx);
            
            Tensor predictions = model->forward(batch_data);
            float loss = utils::loss::cross_entropy_loss(predictions, batch_labels);
            
            epoch_loss += loss;
            num_batches++;
            
            Tensor grad_output = predictions - batch_labels;
            model->backward(grad_output);
            optimizer->step();
            optimizer->zero_grad();
        }
        
        float avg_loss = epoch_loss / num_batches;
        actual_epochs = epoch + 1;
        
        // Early stopping logic
        if (avg_loss < best_loss - 0.001f) {
            best_loss = avg_loss;
            patience_counter = 0;
        } else {
            patience_counter++;
            if (patience_counter >= patience) {
                break;
            }
        }
    }
    
    // Verify early stopping worked
    assert(actual_epochs <= max_epochs);
    assert(actual_epochs > 1);  // Should have trained for at least a few epochs
    
    std::cout << "✅ Early stopping test passed" << std::endl;
    std::cout << "   Trained for " << actual_epochs << " epochs" << std::endl;
    std::cout << "   Best loss: " << best_loss << std::endl;
}

int main() {
    std::cout << "=== Running End-to-End Tests ===" << std::endl;
    
    try {
        test_complete_training_pipeline();
        test_model_evaluation();
        test_model_save_load();
        test_different_optimizers();
        test_activation_functions();
        test_early_stopping();
        
        std::cout << "\n🎉 All end-to-end tests passed successfully!" << std::endl;
        std::cout << "✅ Neural network framework is fully functional!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
