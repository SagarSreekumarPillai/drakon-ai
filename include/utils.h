#ifndef UTILS_H
#define UTILS_H

#include "tensor.h"
#include <string>
#include <vector>
#include <random>
#include <chrono>

/**
 * @brief Utility functions for neural network operations
 */
namespace utils {

/**
 * @brief Random number generator utilities
 */
namespace random {
    
    /**
     * @brief Get a random number generator
     * @return Reference to the random number generator
     */
    std::mt19937& get_rng();
    
    /**
     * @brief Set the random seed
     * @param seed Random seed
     */
    void set_seed(unsigned int seed);
    
    /**
     * @brief Generate random float between min and max
     * @param min Minimum value
     * @param max Maximum value
     * @return Random float
     */
    float random_float(float min = 0.0f, float max = 1.0f);
    
    /**
     * @brief Generate random integers between min and max
     * @param min Minimum value
     * @param max Maximum value
     * @param count Number of integers to generate
     * @return Vector of random integers
     */
    std::vector<int> random_ints(int min, int max, size_t count);
    
    /**
     * @brief Generate random tensor with normal distribution
     * @param shape Tensor shape
     * @param mean Mean of the distribution
     * @param stddev Standard deviation of the distribution
     * @return Random tensor
     */
    Tensor random_normal(const std::vector<size_t>& shape, float mean = 0.0f, float stddev = 1.0f);
    
    /**
     * @brief Generate random tensor with uniform distribution
     * @param shape Tensor shape
     * @param min Minimum value
     * @param max Maximum value
     * @return Random tensor
     */
    Tensor random_uniform(const std::vector<size_t>& shape, float min = -1.0f, float max = 1.0f);
    
    /**
     * @brief Generate random tensor with Xavier initialization
     * @param shape Tensor shape
     * @return Random tensor with Xavier initialization
     */
    Tensor xavier_init(const std::vector<size_t>& shape);
    
    /**
     * @brief Generate random tensor with He initialization
     * @param shape Tensor shape
     * @return Random tensor with He initialization
     */
    Tensor he_init(const std::vector<size_t>& shape);
    
} // namespace random

/**
 * @brief Mathematical utility functions
 */
namespace math {
    
    /**
     * @brief Compute mean of tensor
     * @param t Input tensor
     * @return Mean value
     */
    float mean(const Tensor& t);
    
    /**
     * @brief Compute variance of tensor
     * @param t Input tensor
     * @return Variance value
     */
    float variance(const Tensor& t);
    
    /**
     * @brief Compute standard deviation of tensor
     * @param t Input tensor
     * @return Standard deviation value
     */
    float stddev(const Tensor& t);
    
    /**
     * @brief Compute minimum value in tensor
     * @param t Input tensor
     * @return Minimum value
     */
    float min(const Tensor& t);
    
    /**
     * @brief Compute maximum value in tensor
     * @param t Input tensor
     * @return Maximum value
     */
    float max(const Tensor& t);
    
    /**
     * @brief Compute sum of tensor
     * @param t Input tensor
     * @return Sum value
     */
    float sum(const Tensor& t);
    
    /**
     * @brief Compute L2 norm of tensor
     * @param t Input tensor
     * @return L2 norm
     */
    float l2_norm(const Tensor& t);
    
    /**
     * @brief Compute dot product of two tensors
     * @param a First tensor
     * @param b Second tensor
     * @return Dot product
     */
    float dot_product(const Tensor& a, const Tensor& b);
    
    /**
     * @brief Compute cosine similarity between two tensors
     * @param a First tensor
     * @param b Second tensor
     * @return Cosine similarity
     */
    float cosine_similarity(const Tensor& a, const Tensor& b);
    
} // namespace math

/**
 * @brief Loss function utilities
 */
namespace loss {
    
    /**
     * @brief Mean Squared Error loss
     * @param predictions Predicted values
     * @param targets Target values
     * @return MSE loss
     */
    float mse_loss(const Tensor& predictions, const Tensor& targets);
    
    /**
     * @brief Cross-entropy loss for classification
     * @param predictions Predicted probabilities
     * @param targets Target labels (one-hot encoded)
     * @return Cross-entropy loss
     */
    float cross_entropy_loss(const Tensor& predictions, const Tensor& targets);
    
    /**
     * @brief Binary cross-entropy loss
     * @param predictions Predicted probabilities
     * @param targets Target labels
     * @return Binary cross-entropy loss
     */
    float binary_cross_entropy_loss(const Tensor& predictions, const Tensor& targets);
    
    /**
     * @brief Categorical cross-entropy loss
     * @param predictions Predicted probabilities
     * @param targets Target labels (one-hot encoded)
     * @return Categorical cross-entropy loss
     */
    float categorical_cross_entropy_loss(const Tensor& predictions, const Tensor& targets);
    
} // namespace loss

/**
 * @brief Metrics for evaluation
 */
namespace metrics {
    
    /**
     * @brief Compute accuracy for classification
     * @param predictions Predicted classes
     * @param targets Target classes
     * @return Accuracy (0.0 to 1.0)
     */
    float accuracy(const Tensor& predictions, const Tensor& targets);
    
    /**
     * @brief Compute precision for binary classification
     * @param predictions Predicted classes
     * @param targets Target classes
     * @return Precision
     */
    float precision(const Tensor& predictions, const Tensor& targets);
    
    /**
     * @brief Compute recall for binary classification
     * @param predictions Predicted classes
     * @param targets Target classes
     * @return Recall
     */
    float recall(const Tensor& predictions, const Tensor& targets);
    
    /**
     * @brief Compute F1 score for binary classification
     * @param predictions Predicted classes
     * @param targets Target classes
     * @return F1 score
     */
    float f1_score(const Tensor& predictions, const Tensor& targets);
    
    /**
     * @brief Compute R-squared for regression
     * @param predictions Predicted values
     * @param targets Target values
     * @return R-squared value
     */
    float r_squared(const Tensor& predictions, const Tensor& targets);
    
} // namespace metrics

/**
 * @brief File I/O utilities
 */
namespace io {
    
    /**
     * @brief Save tensor to binary file
     * @param tensor Tensor to save
     * @param filename Output filename
     * @return True if successful
     */
    bool save_tensor_binary(const Tensor& tensor, const std::string& filename);
    
    /**
     * @brief Load tensor from binary file
     * @param filename Input filename
     * @return Loaded tensor
     */
    Tensor load_tensor_binary(const std::string& filename);
    
    /**
     * @brief Save tensor to text file
     * @param tensor Tensor to save
     * @param filename Output filename
     * @return True if successful
     */
    bool save_tensor_text(const Tensor& tensor, const std::string& filename);
    
    /**
     * @brief Load tensor from text file
     * @param filename Input filename
     * @return Loaded tensor
     */
    Tensor load_tensor_text(const std::string& filename);
    
    /**
     * @brief Check if file exists
     * @param filename File path
     * @return True if file exists
     */
    bool file_exists(const std::string& filename);
    
    /**
     * @brief Create directory if it doesn't exist
     * @param dirname Directory path
     * @return True if successful
     */
    bool create_directory(const std::string& dirname);
    
} // namespace io

/**
 * @brief Timing utilities
 */
namespace timing {
    
    /**
     * @brief Timer class for measuring execution time
     */
    class Timer {
    private:
        std::chrono::high_resolution_clock::time_point start_time;
        std::chrono::high_resolution_clock::time_point end_time;
        bool running;
        
    public:
        Timer();
        
        /**
         * @brief Start the timer
         */
        void start();
        
        /**
         * @brief Stop the timer
         */
        void stop();
        
        /**
         * @brief Get elapsed time in milliseconds
         * @return Elapsed time in ms
         */
        double elapsed_ms() const;
        
        /**
         * @brief Get elapsed time in seconds
         * @return Elapsed time in seconds
         */
        double elapsed_seconds() const;
        
        /**
         * @brief Reset the timer
         */
        void reset();
    };
    
} // namespace timing

/**
 * @brief String utilities
 */
namespace string {
    
    /**
     * @brief Split string by delimiter
     * @param str Input string
     * @param delimiter Delimiter character
     * @return Vector of substrings
     */
    std::vector<std::string> split(const std::string& str, char delimiter);
    
    /**
     * @brief Trim whitespace from string
     * @param str Input string
     * @return Trimmed string
     */
    std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * @param str Input string
     * @return Lowercase string
     */
    std::string to_lower(const std::string& str);
    
    /**
     * @brief Convert string to uppercase
     * @param str Input string
     * @return Uppercase string
     */
    std::string to_upper(const std::string& str);
    
    /**
     * @brief Check if string starts with prefix
     * @param str Input string
     * @param prefix Prefix to check
     * @return True if string starts with prefix
     */
    bool starts_with(const std::string& str, const std::string& prefix);
    
    /**
     * @brief Check if string ends with suffix
     * @param str Input string
     * @param suffix Suffix to check
     * @return True if string ends with suffix
     */
    bool ends_with(const std::string& str, const std::string& suffix);
    
} // namespace string

} // namespace utils

#endif // UTILS_H
