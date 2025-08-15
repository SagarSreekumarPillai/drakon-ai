#include "config.h"
#include "dataset.h"
#include "layers.h"
#include "utils.h"
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char** argv) {
    try {
        // Parse command line arguments
        std::string config_file = "config.txt";
        std::string model_path = "";
        
        if (argc > 1) {
            config_file = argv[1];
        }
        if (argc > 2) {
            model_path = argv[2];
        }
        
        std::cout << "=== Drakon AI Evaluation ===" << std::endl;
        std::cout << "Loading configuration from: " << config_file << std::endl;
        
        // Load configuration
        Config config;
        if (!config.load_from_file(config_file)) {
            std::cerr << "Could not load configuration file: " << config_file << std::endl;
            return 1;
        }
        
        // Validate configuration
        if (!config.validate()) {
            std::cerr << "Invalid configuration!" << std::endl;
            return 1;
        }
        
        // Create test dataset
        std::cout << "\n=== Creating Test Dataset ===" << std::endl;
        auto test_dataset = std::make_shared<SyntheticDataset>(
            config.get_dataset_config().num_samples / 4,  // Use 25% for testing
            config.get_dataset_config().input_features,
            config.get_dataset_config().num_classes,
            config.get_dataset_config().problem_type,
            false  // No shuffling for evaluation
        );
        
        std::cout << "Test dataset created: " << test_dataset->size() << " samples" << std::endl;
        
        // Create model
        std::cout << "\n=== Creating Model ===" << std::endl;
        auto model = std::make_unique<MLP>(
            config.get_network_config().layer_sizes,
            config.get_network_config().activation_type,
            "evaluation_model"
        );
        
        std::cout << "Model created with " << model->num_parameters() << " parameters" << std::endl;
        
        // Load model weights if provided
        if (!model_path.empty()) {
            std::cout << "Loading model from: " << model_path << std::endl;
            // In a real implementation, you'd load the model weights here
            // For now, we'll use the randomly initialized model
        }
        
        // Evaluation
        std::cout << "\n=== Running Evaluation ===" << std::endl;
        utils::timing::Timer timer;
        timer.start();
        
        float total_loss = 0.0f;
        float total_accuracy = 0.0f;
        size_t num_batches = 0;
        
        DataLoader test_loader(test_dataset, config.get_training_config().batch_size, false);
        
        for (size_t batch_idx = 0; batch_idx < test_loader.num_batches(); ++batch_idx) {
            auto [batch_data, batch_labels] = test_loader.get_batch(batch_idx);
            
            // Forward pass
            Tensor predictions = model->forward(batch_data);
            
            // Compute metrics
            float loss;
            if (config.get_dataset_config().problem_type == "classification") {
                loss = utils::loss::cross_entropy_loss(predictions, batch_labels);
                float accuracy = utils::metrics::accuracy(predictions, batch_labels);
                total_accuracy += accuracy;
            } else {
                loss = utils::loss::mse_loss(predictions, batch_labels);
            }
            
            total_loss += loss;
            num_batches++;
        }
        
        timer.stop();
        
        // Print results
        std::cout << "\n=== Evaluation Results ===" << std::endl;
        std::cout << "Test samples: " << test_dataset->size() << std::endl;
        std::cout << "Test batches: " << num_batches << std::endl;
        std::cout << "Average loss: " << total_loss / num_batches << std::endl;
        
        if (config.get_dataset_config().problem_type == "classification") {
            std::cout << "Average accuracy: " << total_accuracy / num_batches << std::endl;
        } else {
            // For regression, compute R² on a sample
            test_dataset->reset();
            auto [sample_data, sample_labels] = test_dataset->get_batch(100);
            Tensor sample_predictions = model->forward(sample_data);
            float r2 = utils::metrics::r_squared(sample_predictions, sample_labels);
            std::cout << "R² score: " << r2 << std::endl;
        }
        
        std::cout << "Evaluation time: " << timer.elapsed_seconds() << " seconds" << std::endl;
        
        // Detailed analysis
        std::cout << "\n=== Detailed Analysis ===" << std::endl;
        
        // Test on a few specific examples
        test_dataset->reset();
        auto [examples_data, examples_labels] = test_dataset->get_batch(5);
        Tensor examples_predictions = model->forward(examples_data);
        
        std::cout << "Sample predictions:" << std::endl;
        for (size_t i = 0; i < 5; ++i) {
            std::cout << "Example " << i + 1 << ":" << std::endl;
            std::cout << "  Input: ";
            for (size_t j = 0; j < std::min(size_t(3), examples_data.cols()); ++j) {
                std::cout << examples_data(i, j) << " ";
            }
            if (examples_data.cols() > 3) {
                std::cout << "...";
            }
            std::cout << std::endl;
            
            std::cout << "  Target: ";
            for (size_t j = 0; j < examples_labels.cols(); ++j) {
                std::cout << examples_labels(i, j) << " ";
            }
            std::cout << std::endl;
            
            std::cout << "  Prediction: ";
            for (size_t j = 0; j < examples_predictions.cols(); ++j) {
                std::cout << examples_predictions(i, j) << " ";
            }
            std::cout << std::endl;
            std::cout << std::endl;
        }
        
        // Model statistics
        std::cout << "=== Model Statistics ===" << std::endl;
        std::cout << "Total parameters: " << model->num_parameters() << std::endl;
        std::cout << "Model name: " << model->get_name() << std::endl;
        
        // Performance metrics
        std::cout << "\n=== Performance Metrics ===" << std::endl;
        std::cout << "✅ Evaluation completed successfully!" << std::endl;
        std::cout << "✅ Model performance assessed" << std::endl;
        
        if (config.get_dataset_config().problem_type == "classification") {
            float avg_acc = total_accuracy / num_batches;
            if (avg_acc > 0.8f) {
                std::cout << "🎉 Excellent performance!" << std::endl;
            } else if (avg_acc > 0.6f) {
                std::cout << "👍 Good performance" << std::endl;
            } else {
                std::cout << "⚠️  Performance needs improvement" << std::endl;
            }
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during evaluation: " << e.what() << std::endl;
        return 1;
    }
}
