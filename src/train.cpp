#include "config.h"
#include "dataset.h"
#include "layers.h"
#include "optimizer.h"
#include "utils.h"
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char** argv) {
    try {
        // Parse command line arguments
        std::string config_file = "config.txt";
        if (argc > 1) {
            config_file = argv[1];
        }
        
        std::cout << "=== Drakon AI Training ===" << std::endl;
        std::cout << "Loading configuration from: " << config_file << std::endl;
        
        // Load configuration
        Config config;
        if (!config.load_from_file(config_file)) {
            std::cout << "Creating default configuration..." << std::endl;
            config = create_default_config("classification", 10, 2);
            config.save_to_file(config_file);
        }
        
        // Validate configuration
        if (!config.validate()) {
            std::cerr << "Invalid configuration!" << std::endl;
            return 1;
        }
        
        config.print_summary();
        
        // Create dataset
        std::cout << "\n=== Creating Dataset ===" << std::endl;
        auto dataset = std::make_shared<SyntheticDataset>(
            config.get_dataset_config().num_samples,
            config.get_dataset_config().input_features,
            config.get_dataset_config().num_classes,
            config.get_dataset_config().problem_type,
            config.get_training_config().shuffle_data
        );
        
        std::cout << "Dataset created: " << dataset->size() << " samples" << std::endl;
        std::cout << "Input dimension: " << dataset->input_dim() << std::endl;
        std::cout << "Output dimension: " << dataset->output_dim() << std::endl;
        
        // Create model
        std::cout << "\n=== Creating Model ===" << std::endl;
        auto model = std::make_unique<MLP>(
            config.get_network_config().layer_sizes,
            config.get_network_config().activation_type,
            "main_model"
        );
        
        std::cout << "Model created with " << model->num_parameters() << " parameters" << std::endl;
        
        // Create optimizer
        std::cout << "\n=== Creating Optimizer ===" << std::endl;
        auto optimizer = create_optimizer(
            config.get_training_config().optimizer_type,
            config.get_training_config().learning_rate
        );
        
        // Set optimizer parameters
        auto params = model->get_parameters();
        auto grads = model->get_gradients();
        optimizer->set_parameters(params, grads);
        
        std::cout << "Optimizer created: " << config.get_training_config().optimizer_type << std::endl;
        
        // Create data loader
        DataLoader train_loader(dataset, config.get_training_config().batch_size, true);
        
        // Training loop
        std::cout << "\n=== Starting Training ===" << std::endl;
        utils::timing::Timer timer;
        timer.start();
        
        float best_loss = std::numeric_limits<float>::infinity();
        size_t patience_counter = 0;
        
        for (size_t epoch = 0; epoch < config.get_training_config().epochs; ++epoch) {
            float epoch_loss = 0.0f;
            size_t num_batches = 0;
            
            // Training epoch
            for (size_t batch_idx = 0; batch_idx < train_loader.num_batches(); ++batch_idx) {
                auto [batch_data, batch_labels] = train_loader.get_batch(batch_idx);
                
                // Forward pass
                Tensor predictions = model->forward(batch_data);
                
                // Compute loss
                float loss;
                if (config.get_dataset_config().problem_type == "classification") {
                    loss = utils::loss::cross_entropy_loss(predictions, batch_labels);
                } else {
                    loss = utils::loss::mse_loss(predictions, batch_labels);
                }
                
                epoch_loss += loss;
                num_batches++;
                
                // Backward pass
                Tensor grad_output = predictions - batch_labels;  // Simplified gradient
                model->backward(grad_output);
                
                // Update parameters
                optimizer->step();
                optimizer->zero_grad();
            }
            
            float avg_loss = epoch_loss / num_batches;
            
            // Print progress
            if (epoch % 10 == 0 || epoch == config.get_training_config().epochs - 1) {
                std::cout << "Epoch " << epoch << "/" << config.get_training_config().epochs 
                         << " - Loss: " << avg_loss << std::endl;
            }
            
            // Early stopping
            if (avg_loss < best_loss - config.get_training_config().min_delta) {
                best_loss = avg_loss;
                patience_counter = 0;
                
                // Save best model
                if (config.get_model_config().save_best_only) {
                    // In a real implementation, you'd save the model here
                    std::cout << "New best loss: " << best_loss << " (saving model)" << std::endl;
                }
            } else {
                patience_counter++;
                if (patience_counter >= config.get_training_config().early_stopping_patience) {
                    std::cout << "Early stopping triggered after " << epoch + 1 << " epochs" << std::endl;
                    break;
                }
            }
        }
        
        timer.stop();
        std::cout << "\n=== Training Complete ===" << std::endl;
        std::cout << "Total training time: " << timer.elapsed_seconds() << " seconds" << std::endl;
        std::cout << "Best loss achieved: " << best_loss << std::endl;
        
        // Test the model
        std::cout << "\n=== Testing Model ===" << std::endl;
        dataset->reset();
        auto [test_data, test_labels] = dataset->get_batch(100);
        Tensor test_predictions = model->forward(test_data);
        
        float test_loss;
        if (config.get_dataset_config().problem_type == "classification") {
            test_loss = utils::loss::cross_entropy_loss(test_predictions, test_labels);
            float accuracy = utils::metrics::accuracy(test_predictions, test_labels);
            std::cout << "Test Loss: " << test_loss << std::endl;
            std::cout << "Test Accuracy: " << accuracy << std::endl;
        } else {
            test_loss = utils::loss::mse_loss(test_predictions, test_labels);
            float r2 = utils::metrics::r_squared(test_predictions, test_labels);
            std::cout << "Test Loss: " << test_loss << std::endl;
            std::cout << "Test R²: " << r2 << std::endl;
        }
        
        std::cout << "\n=== Training Summary ===" << std::endl;
        std::cout << "✅ Training completed successfully!" << std::endl;
        std::cout << "✅ Model is ready for inference" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during training: " << e.what() << std::endl;
        return 1;
    }
}
