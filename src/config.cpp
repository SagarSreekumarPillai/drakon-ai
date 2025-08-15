#include "config.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <sstream>

Config::Config(const std::string& config_file) {
    load_from_file(config_file);
}

bool Config::load_from_file(const std::string& config_file) {
    // For now, implement a simple text-based config format
    // In a more sophisticated implementation, this would parse JSON or YAML
    
    std::ifstream file(config_file);
    if (!file.is_open()) {
        std::cerr << "Could not open config file: " << config_file << std::endl;
        return false;
    }
    
    std::string line;
    std::string current_section = "";
    
    while (std::getline(file, line)) {
        line = utils::string::trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Check for section headers
        if (line[0] == '[' && line.back() == ']') {
            current_section = line.substr(1, line.length() - 2);
            continue;
        }
        
        // Parse key-value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = utils::string::trim(line.substr(0, pos));
            std::string value = utils::string::trim(line.substr(pos + 1));
            
            if (current_section == "network") {
                if (key == "layer_sizes") {
                    // Parse comma-separated list of integers
                    std::vector<std::string> tokens = utils::string::split(value, ',');
                    network_config.layer_sizes.clear();
                    for (const auto& token : tokens) {
                        network_config.layer_sizes.push_back(std::stoul(utils::string::trim(token)));
                    }
                } else if (key == "activation_type") {
                    network_config.activation_type = value;
                } else if (key == "output_activation") {
                    network_config.output_activation = value;
                } else if (key == "use_dropout") {
                    network_config.use_dropout = (value == "true");
                } else if (key == "dropout_rate") {
                    network_config.dropout_rate = std::stof(value);
                } else if (key == "weight_init") {
                    network_config.weight_init = value;
                }
            } else if (current_section == "training") {
                if (key == "epochs") {
                    training_config.epochs = std::stoul(value);
                } else if (key == "batch_size") {
                    training_config.batch_size = std::stoul(value);
                } else if (key == "learning_rate") {
                    training_config.learning_rate = std::stof(value);
                } else if (key == "optimizer_type") {
                    training_config.optimizer_type = value;
                } else if (key == "momentum") {
                    training_config.momentum = std::stof(value);
                } else if (key == "beta1") {
                    training_config.beta1 = std::stof(value);
                } else if (key == "beta2") {
                    training_config.beta2 = std::stof(value);
                } else if (key == "alpha") {
                    training_config.alpha = std::stof(value);
                } else if (key == "epsilon") {
                    training_config.epsilon = std::stof(value);
                } else if (key == "shuffle_data") {
                    training_config.shuffle_data = (value == "true");
                } else if (key == "validation_split") {
                    training_config.validation_split = std::stof(value);
                } else if (key == "early_stopping_patience") {
                    training_config.early_stopping_patience = std::stoul(value);
                } else if (key == "min_delta") {
                    training_config.min_delta = std::stof(value);
                }
            } else if (current_section == "dataset") {
                if (key == "type") {
                    dataset_config.type = value;
                } else if (key == "num_samples") {
                    dataset_config.num_samples = std::stoul(value);
                } else if (key == "input_features") {
                    dataset_config.input_features = std::stoul(value);
                } else if (key == "num_classes") {
                    dataset_config.num_classes = std::stoul(value);
                } else if (key == "problem_type") {
                    dataset_config.problem_type = value;
                } else if (key == "file_path") {
                    dataset_config.file_path = value;
                } else if (key == "has_header") {
                    dataset_config.has_header = (value == "true");
                } else if (key == "normalize_data") {
                    dataset_config.normalize_data = (value == "true");
                } else if (key == "standardize_data") {
                    dataset_config.standardize_data = (value == "true");
                }
            } else if (current_section == "model") {
                if (key == "save_path") {
                    model_config.save_path = value;
                } else if (key == "load_path") {
                    model_config.load_path = value;
                } else if (key == "save_best_only") {
                    model_config.save_best_only = (value == "true");
                } else if (key == "save_format") {
                    model_config.save_format = value;
                } else if (key == "save_optimizer_state") {
                    model_config.save_optimizer_state = (value == "true");
                }
            } else {
                // Custom parameter
                custom_params[key] = value;
            }
        }
    }
    
    return true;
}

bool Config::save_to_file(const std::string& config_file) const {
    std::ofstream file(config_file);
    if (!file.is_open()) {
        std::cerr << "Could not create config file: " << config_file << std::endl;
        return false;
    }
    
    // Network section
    file << "[network]\n";
    file << "layer_sizes = ";
    for (size_t i = 0; i < network_config.layer_sizes.size(); ++i) {
        if (i > 0) file << ", ";
        file << network_config.layer_sizes[i];
    }
    file << "\n";
    file << "activation_type = " << network_config.activation_type << "\n";
    file << "output_activation = " << network_config.output_activation << "\n";
    file << "use_dropout = " << (network_config.use_dropout ? "true" : "false") << "\n";
    file << "dropout_rate = " << network_config.dropout_rate << "\n";
    file << "weight_init = " << network_config.weight_init << "\n\n";
    
    // Training section
    file << "[training]\n";
    file << "epochs = " << training_config.epochs << "\n";
    file << "batch_size = " << training_config.batch_size << "\n";
    file << "learning_rate = " << training_config.learning_rate << "\n";
    file << "optimizer_type = " << training_config.optimizer_type << "\n";
    file << "momentum = " << training_config.momentum << "\n";
    file << "beta1 = " << training_config.beta1 << "\n";
    file << "beta2 = " << training_config.beta2 << "\n";
    file << "alpha = " << training_config.alpha << "\n";
    file << "epsilon = " << training_config.epsilon << "\n";
    file << "shuffle_data = " << (training_config.shuffle_data ? "true" : "false") << "\n";
    file << "validation_split = " << training_config.validation_split << "\n";
    file << "early_stopping_patience = " << training_config.early_stopping_patience << "\n";
    file << "min_delta = " << training_config.min_delta << "\n\n";
    
    // Dataset section
    file << "[dataset]\n";
    file << "type = " << dataset_config.type << "\n";
    file << "num_samples = " << dataset_config.num_samples << "\n";
    file << "input_features = " << dataset_config.input_features << "\n";
    file << "num_classes = " << dataset_config.num_classes << "\n";
    file << "problem_type = " << dataset_config.problem_type << "\n";
    file << "file_path = " << dataset_config.file_path << "\n";
    file << "has_header = " << (dataset_config.has_header ? "true" : "false") << "\n";
    file << "normalize_data = " << (dataset_config.normalize_data ? "true" : "false") << "\n";
    file << "standardize_data = " << (dataset_config.standardize_data ? "true" : "false") << "\n\n";
    
    // Model section
    file << "[model]\n";
    file << "save_path = " << model_config.save_path << "\n";
    file << "load_path = " << model_config.load_path << "\n";
    file << "save_best_only = " << (model_config.save_best_only ? "true" : "false") << "\n";
    file << "save_format = " << model_config.save_format << "\n";
    file << "save_optimizer_state = " << (model_config.save_optimizer_state ? "true" : "false") << "\n\n";
    
    // Custom parameters
    if (!custom_params.empty()) {
        file << "[custom]\n";
        for (const auto& param : custom_params) {
            file << param.first << " = " << param.second << "\n";
        }
    }
    
    return true;
}

std::string Config::get_custom_param(const std::string& key, const std::string& default_value) const {
    auto it = custom_params.find(key);
    return (it != custom_params.end()) ? it->second : default_value;
}

void Config::set_custom_param(const std::string& key, const std::string& value) {
    custom_params[key] = value;
}

bool Config::validate() const {
    // Basic validation checks
    if (network_config.layer_sizes.size() < 2) {
        std::cerr << "Network must have at least 2 layers (input and output)" << std::endl;
        return false;
    }
    
    if (training_config.learning_rate <= 0.0f) {
        std::cerr << "Learning rate must be positive" << std::endl;
        return false;
    }
    
    if (training_config.batch_size == 0) {
        std::cerr << "Batch size must be positive" << std::endl;
        return false;
    }
    
    if (training_config.validation_split < 0.0f || training_config.validation_split > 1.0f) {
        std::cerr << "Validation split must be between 0.0 and 1.0" << std::endl;
        return false;
    }
    
    if (network_config.dropout_rate < 0.0f || network_config.dropout_rate > 1.0f) {
        std::cerr << "Dropout rate must be between 0.0 and 1.0" << std::endl;
        return false;
    }
    
    return true;
}

void Config::print_summary() const {
    std::cout << "=== Configuration Summary ===" << std::endl;
    
    std::cout << "\n[Network]" << std::endl;
    std::cout << "Layer sizes: ";
    for (size_t i = 0; i < network_config.layer_sizes.size(); ++i) {
        if (i > 0) std::cout << " -> ";
        std::cout << network_config.layer_sizes[i];
    }
    std::cout << std::endl;
    std::cout << "Activation: " << network_config.activation_type << std::endl;
    std::cout << "Output activation: " << network_config.output_activation << std::endl;
    std::cout << "Dropout: " << (network_config.use_dropout ? "Yes" : "No");
    if (network_config.use_dropout) {
        std::cout << " (rate: " << network_config.dropout_rate << ")";
    }
    std::cout << std::endl;
    
    std::cout << "\n[Training]" << std::endl;
    std::cout << "Epochs: " << training_config.epochs << std::endl;
    std::cout << "Batch size: " << training_config.batch_size << std::endl;
    std::cout << "Learning rate: " << training_config.learning_rate << std::endl;
    std::cout << "Optimizer: " << training_config.optimizer_type << std::endl;
    std::cout << "Validation split: " << training_config.validation_split << std::endl;
    
    std::cout << "\n[Dataset]" << std::endl;
    std::cout << "Type: " << dataset_config.type << std::endl;
    std::cout << "Samples: " << dataset_config.num_samples << std::endl;
    std::cout << "Input features: " << dataset_config.input_features << std::endl;
    std::cout << "Classes: " << dataset_config.num_classes << std::endl;
    std::cout << "Problem type: " << dataset_config.problem_type << std::endl;
    
    std::cout << "\n[Model]" << std::endl;
    std::cout << "Save path: " << model_config.save_path << std::endl;
    std::cout << "Load path: " << model_config.load_path << std::endl;
    std::cout << "Save format: " << model_config.save_format << std::endl;
    
    std::cout << "=============================" << std::endl;
}

Config create_default_config(const std::string& task_type, size_t input_dim, size_t output_dim) {
    Config config;
    
    // Set network configuration
    NetworkConfig net_config;
    if (task_type == "classification") {
        net_config.layer_sizes = {input_dim, 64, 32, output_dim};
        net_config.activation_type = "relu";
        net_config.output_activation = "softmax";
    } else if (task_type == "regression") {
        net_config.layer_sizes = {input_dim, 64, 32, output_dim};
        net_config.activation_type = "relu";
        net_config.output_activation = "linear";
    }
    net_config.use_dropout = true;
    net_config.dropout_rate = 0.3f;
    net_config.weight_init = "xavier";
    config.set_network_config(net_config);
    
    // Set training configuration
    TrainingConfig train_config;
    train_config.epochs = 100;
    train_config.batch_size = 32;
    train_config.learning_rate = 0.001f;
    train_config.optimizer_type = "adam";
    train_config.validation_split = 0.2f;
    train_config.early_stopping_patience = 10;
    config.set_training_config(train_config);
    
    // Set dataset configuration
    DatasetConfig data_config;
    data_config.type = "synthetic";
    data_config.num_samples = 1000;
    data_config.input_features = input_dim;
    data_config.num_classes = output_dim;
    data_config.problem_type = task_type;
    data_config.standardize_data = true;
    config.set_dataset_config(data_config);
    
    // Set model configuration
    ModelConfig model_config;
    model_config.save_path = "model.bin";
    model_config.save_format = "binary";
    model_config.save_best_only = true;
    config.set_model_config(model_config);
    
    return config;
}
