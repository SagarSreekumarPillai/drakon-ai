#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <map>

/**
 * @brief Configuration for neural network architecture
 */
struct NetworkConfig {
    std::vector<size_t> layer_sizes;  // Input, hidden, output layer sizes
    std::string activation_type;      // "relu", "sigmoid", "tanh"
    std::string output_activation;    // "softmax", "sigmoid", "linear"
    bool use_dropout;
    float dropout_rate;
    std::string weight_init;          // "xavier", "he", "normal", "uniform"
    
    NetworkConfig() : activation_type("relu"), output_activation("softmax"),
                     use_dropout(false), dropout_rate(0.5f), weight_init("xavier") {}
};

/**
 * @brief Configuration for training
 */
struct TrainingConfig {
    size_t epochs;
    size_t batch_size;
    float learning_rate;
    std::string optimizer_type;       // "sgd", "adam", "rmsprop"
    float momentum;                   // For SGD
    float beta1, beta2;               // For Adam
    float alpha;                      // For RMSprop
    float epsilon;                    // For Adam/RMSprop
    bool shuffle_data;
    float validation_split;
    size_t early_stopping_patience;
    float min_delta;
    
    TrainingConfig() : epochs(100), batch_size(32), learning_rate(0.001f),
                      optimizer_type("adam"), momentum(0.9f), beta1(0.9f), beta2(0.999f),
                      alpha(0.99f), epsilon(1e-8f), shuffle_data(true), validation_split(0.2f),
                      early_stopping_patience(10), min_delta(1e-4f) {}
};

/**
 * @brief Configuration for dataset
 */
struct DatasetConfig {
    std::string type;                 // "synthetic", "csv", "custom"
    size_t num_samples;               // For synthetic datasets
    size_t input_features;
    size_t num_classes;
    std::string problem_type;         // "classification", "regression"
    std::string file_path;            // For CSV datasets
    std::vector<size_t> feature_columns;
    size_t label_column;
    bool has_header;
    bool normalize_data;
    bool standardize_data;
    
    DatasetConfig() : type("synthetic"), num_samples(1000), input_features(10),
                     num_classes(2), problem_type("classification"), has_header(true),
                     normalize_data(false), standardize_data(true) {}
};

/**
 * @brief Configuration for model saving/loading
 */
struct ModelConfig {
    std::string save_path;
    std::string load_path;
    bool save_best_only;
    std::string save_format;          // "binary", "json"
    bool save_optimizer_state;
    
    ModelConfig() : save_path("model.bin"), load_path(""), save_best_only(true),
                   save_format("binary"), save_optimizer_state(true) {}
};

/**
 * @brief Main configuration class that holds all configurations
 */
class Config {
private:
    NetworkConfig network_config;
    TrainingConfig training_config;
    DatasetConfig dataset_config;
    ModelConfig model_config;
    std::map<std::string, std::string> custom_params;
    
public:
    /**
     * @brief Default constructor
     */
    Config() = default;
    
    /**
     * @brief Constructor from configuration file
     * @param config_file Path to configuration file
     */
    explicit Config(const std::string& config_file);
    
    /**
     * @brief Load configuration from file
     * @param config_file Path to configuration file
     * @return True if loading was successful
     */
    bool load_from_file(const std::string& config_file);
    
    /**
     * @brief Save configuration to file
     * @param config_file Path to configuration file
     * @return True if saving was successful
     */
    bool save_to_file(const std::string& config_file) const;
    
    /**
     * @brief Get network configuration
     * @return Reference to network configuration
     */
    const NetworkConfig& get_network_config() const { return network_config; }
    
    /**
     * @brief Get training configuration
     * @return Reference to training configuration
     */
    const TrainingConfig& get_training_config() const { return training_config; }
    
    /**
     * @brief Get dataset configuration
     * @return Reference to dataset configuration
     */
    const DatasetConfig& get_dataset_config() const { return dataset_config; }
    
    /**
     * @brief Get model configuration
     * @return Reference to model configuration
     */
    const ModelConfig& get_model_config() const { return model_config; }
    
    /**
     * @brief Set network configuration
     * @param config Network configuration
     */
    void set_network_config(const NetworkConfig& config) { network_config = config; }
    
    /**
     * @brief Set training configuration
     * @param config Training configuration
     */
    void set_training_config(const TrainingConfig& config) { training_config = config; }
    
    /**
     * @brief Set dataset configuration
     * @param config Dataset configuration
     */
    void set_dataset_config(const DatasetConfig& config) { dataset_config = config; }
    
    /**
     * @brief Set model configuration
     * @param config Model configuration
     */
    void set_model_config(const ModelConfig& config) { model_config = config; }
    
    /**
     * @brief Get custom parameter
     * @param key Parameter key
     * @param default_value Default value if key not found
     * @return Parameter value
     */
    std::string get_custom_param(const std::string& key, const std::string& default_value = "") const;
    
    /**
     * @brief Set custom parameter
     * @param key Parameter key
     * @param value Parameter value
     */
    void set_custom_param(const std::string& key, const std::string& value);
    
    /**
     * @brief Validate configuration
     * @return True if configuration is valid
     */
    bool validate() const;
    
    /**
     * @brief Print configuration summary
     */
    void print_summary() const;
};

/**
 * @brief Create default configuration for common tasks
 * @param task_type Type of task ("classification", "regression")
 * @param input_dim Input dimension
 * @param output_dim Output dimension
 * @return Default configuration
 */
Config create_default_config(const std::string& task_type, size_t input_dim, size_t output_dim);

#endif // CONFIG_H
