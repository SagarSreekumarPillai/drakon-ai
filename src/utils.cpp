#include "utils.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <cstring>

namespace utils {

// Random utilities
namespace random {

std::mt19937& get_rng() {
    static std::mt19937 rng(std::random_device{}());
    return rng;
}

void set_seed(unsigned int seed) {
    get_rng().seed(seed);
}

float random_float(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(get_rng());
}

std::vector<int> random_ints(int min, int max, size_t count) {
    std::uniform_int_distribution<int> dist(min, max);
    std::vector<int> result(count);
    for (size_t i = 0; i < count; ++i) {
        result[i] = dist(get_rng());
    }
    return result;
}

Tensor random_normal(const std::vector<size_t>& shape, float mean, float stddev) {
    Tensor result(shape, 0.0f);
    std::normal_distribution<float> dist(mean, stddev);
    for (float& val : result.data()) {
        val = dist(get_rng());
    }
    return result;
}

Tensor random_uniform(const std::vector<size_t>& shape, float min, float max) {
    Tensor result(shape, 0.0f);
    std::uniform_real_distribution<float> dist(min, max);
    for (float& val : result.data()) {
        val = dist(get_rng());
    }
    return result;
}

Tensor xavier_init(const std::vector<size_t>& shape) {
    if (shape.size() < 2) {
        throw std::invalid_argument("Xavier initialization requires at least 2 dimensions");
    }
    
    size_t fan_in = shape[0];
    size_t fan_out = shape[1];
    float scale = std::sqrt(2.0f / (fan_in + fan_out));
    
    return random_normal(shape, 0.0f, scale);
}

Tensor he_init(const std::vector<size_t>& shape) {
    if (shape.size() < 2) {
        throw std::invalid_argument("He initialization requires at least 2 dimensions");
    }
    
    size_t fan_in = shape[0];
    float scale = std::sqrt(2.0f / fan_in);
    
    return random_normal(shape, 0.0f, scale);
}

} // namespace random

// Mathematical utilities
namespace math {

float mean(const Tensor& t) {
    if (t.size() == 0) return 0.0f;
    
    float sum = 0.0f;
    for (const float& val : t.data()) {
        sum += val;
    }
    return sum / t.size();
}

float variance(const Tensor& t) {
    if (t.size() == 0) return 0.0f;
    
    float mean_val = mean(t);
    float sum_sq_diff = 0.0f;
    
    for (const float& val : t.data()) {
        float diff = val - mean_val;
        sum_sq_diff += diff * diff;
    }
    
    return sum_sq_diff / t.size();
}

float stddev(const Tensor& t) {
    return std::sqrt(variance(t));
}

float min(const Tensor& t) {
    if (t.size() == 0) return 0.0f;
    
    float min_val = t.data()[0];
    for (const float& val : t.data()) {
        min_val = std::min(min_val, val);
    }
    return min_val;
}

float max(const Tensor& t) {
    if (t.size() == 0) return 0.0f;
    
    float max_val = t.data()[0];
    for (const float& val : t.data()) {
        max_val = std::max(max_val, val);
    }
    return max_val;
}

float sum(const Tensor& t) {
    float sum_val = 0.0f;
    for (const float& val : t.data()) {
        sum_val += val;
    }
    return sum_val;
}

float l2_norm(const Tensor& t) {
    float sum_sq = 0.0f;
    for (const float& val : t.data()) {
        sum_sq += val * val;
    }
    return std::sqrt(sum_sq);
}

float dot_product(const Tensor& a, const Tensor& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("Tensors must have the same size for dot product");
    }
    
    float result = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        result += a.data()[i] * b.data()[i];
    }
    return result;
}

float cosine_similarity(const Tensor& a, const Tensor& b) {
    float dot = dot_product(a, b);
    float norm_a = l2_norm(a);
    float norm_b = l2_norm(b);
    
    if (norm_a == 0.0f || norm_b == 0.0f) {
        return 0.0f;
    }
    
    return dot / (norm_a * norm_b);
}

} // namespace math

// Loss function utilities
namespace loss {

float mse_loss(const Tensor& predictions, const Tensor& targets) {
    if (predictions.size() != targets.size()) {
        throw std::invalid_argument("Predictions and targets must have the same size");
    }
    
    float sum_sq_error = 0.0f;
    for (size_t i = 0; i < predictions.size(); ++i) {
        float diff = predictions.data()[i] - targets.data()[i];
        sum_sq_error += diff * diff;
    }
    
    return sum_sq_error / predictions.size();
}

float cross_entropy_loss(const Tensor& predictions, const Tensor& targets) {
    if (predictions.size() != targets.size()) {
        throw std::invalid_argument("Predictions and targets must have the same size");
    }
    
    float loss = 0.0f;
    for (size_t i = 0; i < predictions.size(); ++i) {
        float pred = std::max(std::min(predictions.data()[i], 1.0f - 1e-7f), 1e-7f);
        loss -= targets.data()[i] * std::log(pred);
    }
    
    return loss / predictions.size();
}

float binary_cross_entropy_loss(const Tensor& predictions, const Tensor& targets) {
    if (predictions.size() != targets.size()) {
        throw std::invalid_argument("Predictions and targets must have the same size");
    }
    
    float loss = 0.0f;
    for (size_t i = 0; i < predictions.size(); ++i) {
        float pred = std::max(std::min(predictions.data()[i], 1.0f - 1e-7f), 1e-7f);
        float target = targets.data()[i];
        loss -= target * std::log(pred) + (1.0f - target) * std::log(1.0f - pred);
    }
    
    return loss / predictions.size();
}

float categorical_cross_entropy_loss(const Tensor& predictions, const Tensor& targets) {
    return cross_entropy_loss(predictions, targets);
}

} // namespace loss

// Metrics utilities
namespace metrics {

float accuracy(const Tensor& predictions, const Tensor& targets) {
    if (predictions.size() != targets.size()) {
        throw std::invalid_argument("Predictions and targets must have the same size");
    }
    
    size_t correct = 0;
    for (size_t i = 0; i < predictions.size(); ++i) {
        if (std::round(predictions.data()[i]) == std::round(targets.data()[i])) {
            correct++;
        }
    }
    
    return static_cast<float>(correct) / predictions.size();
}

float precision(const Tensor& predictions, const Tensor& targets) {
    if (predictions.size() != targets.size()) {
        throw std::invalid_argument("Predictions and targets must have the same size");
    }
    
    size_t true_positives = 0;
    size_t false_positives = 0;
    
    for (size_t i = 0; i < predictions.size(); ++i) {
        bool pred = std::round(predictions.data()[i]) > 0.5f;
        bool target = std::round(targets.data()[i]) > 0.5f;
        
        if (pred && target) {
            true_positives++;
        } else if (pred && !target) {
            false_positives++;
        }
    }
    
    if (true_positives + false_positives == 0) {
        return 0.0f;
    }
    
    return static_cast<float>(true_positives) / (true_positives + false_positives);
}

float recall(const Tensor& predictions, const Tensor& targets) {
    if (predictions.size() != targets.size()) {
        throw std::invalid_argument("Predictions and targets must have the same size");
    }
    
    size_t true_positives = 0;
    size_t false_negatives = 0;
    
    for (size_t i = 0; i < predictions.size(); ++i) {
        bool pred = std::round(predictions.data()[i]) > 0.5f;
        bool target = std::round(targets.data()[i]) > 0.5f;
        
        if (pred && target) {
            true_positives++;
        } else if (!pred && target) {
            false_negatives++;
        }
    }
    
    if (true_positives + false_negatives == 0) {
        return 0.0f;
    }
    
    return static_cast<float>(true_positives) / (true_positives + false_negatives);
}

float f1_score(const Tensor& predictions, const Tensor& targets) {
    float prec = precision(predictions, targets);
    float rec = recall(predictions, targets);
    
    if (prec + rec == 0.0f) {
        return 0.0f;
    }
    
    return 2.0f * prec * rec / (prec + rec);
}

float r_squared(const Tensor& predictions, const Tensor& targets) {
    if (predictions.size() != targets.size()) {
        throw std::invalid_argument("Predictions and targets must have the same size");
    }
    
    float target_mean = math::mean(targets);
    float ss_tot = 0.0f;
    float ss_res = 0.0f;
    
    for (size_t i = 0; i < predictions.size(); ++i) {
        float target_diff = targets.data()[i] - target_mean;
        float pred_diff = predictions.data()[i] - targets.data()[i];
        
        ss_tot += target_diff * target_diff;
        ss_res += pred_diff * pred_diff;
    }
    
    if (ss_tot == 0.0f) {
        return 0.0f;
    }
    
    return 1.0f - (ss_res / ss_tot);
}

} // namespace metrics

// File I/O utilities
namespace io {

bool save_tensor_binary(const Tensor& tensor, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Write shape information
    std::vector<size_t> shape = tensor.shape();
    size_t shape_size = shape.size();
    file.write(reinterpret_cast<const char*>(&shape_size), sizeof(shape_size));
    file.write(reinterpret_cast<const char*>(shape.data()), shape_size * sizeof(size_t));
    
    // Write data
    file.write(reinterpret_cast<const char*>(tensor.data().data()), tensor.size() * sizeof(float));
    
    return true;
}

Tensor load_tensor_binary(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    // Read shape information
    size_t shape_size;
    file.read(reinterpret_cast<char*>(&shape_size), sizeof(shape_size));
    
    std::vector<size_t> shape(shape_size);
    file.read(reinterpret_cast<char*>(shape.data()), shape_size * sizeof(size_t));
    
    // Create tensor
    Tensor tensor(shape, 0.0f);
    
    // Read data
    file.read(reinterpret_cast<char*>(tensor.data().data()), tensor.size() * sizeof(float));
    
    return tensor;
}

bool save_tensor_text(const Tensor& tensor, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Write shape
    std::vector<size_t> shape = tensor.shape();
    file << "shape: ";
    for (size_t i = 0; i < shape.size(); ++i) {
        if (i > 0) file << " ";
        file << shape[i];
    }
    file << "\n";
    
    // Write data
    for (size_t i = 0; i < tensor.size(); ++i) {
        if (i > 0) file << " ";
        file << tensor.data()[i];
    }
    file << "\n";
    
    return true;
}

Tensor load_tensor_text(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    std::string line;
    
    // Read shape
    std::getline(file, line);
    if (line.substr(0, 6) != "shape:") {
        throw std::runtime_error("Invalid tensor file format");
    }
    
    std::vector<std::string> shape_tokens = string::split(line.substr(6), ' ');
    std::vector<size_t> shape;
    for (const auto& token : shape_tokens) {
        if (!token.empty()) {
            shape.push_back(std::stoul(token));
        }
    }
    
    // Create tensor
    Tensor tensor(shape, 0.0f);
    
    // Read data
    std::getline(file, line);
    std::vector<std::string> data_tokens = string::split(line, ' ');
    size_t i = 0;
    for (const auto& token : data_tokens) {
        if (!token.empty() && i < tensor.size()) {
            tensor.data()[i++] = std::stof(token);
        }
    }
    
    return tensor;
}

bool file_exists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

bool create_directory(const std::string& dirname) {
    // Simple implementation - in a real system, you'd use mkdir with proper error handling
    std::string command = "mkdir -p " + dirname;
    return system(command.c_str()) == 0;
}

} // namespace io

// Timing utilities
namespace timing {

Timer::Timer() : running(false) {}

void Timer::start() {
    start_time = std::chrono::high_resolution_clock::now();
    running = true;
}

void Timer::stop() {
    if (running) {
        end_time = std::chrono::high_resolution_clock::now();
        running = false;
    }
}

double Timer::elapsed_ms() const {
    auto end = running ? std::chrono::high_resolution_clock::now() : end_time;
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_time);
    return duration.count() / 1000.0;
}

double Timer::elapsed_seconds() const {
    return elapsed_ms() / 1000.0;
}

void Timer::reset() {
    running = false;
}

} // namespace timing

// String utilities
namespace string {

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string to_lower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string to_upper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool starts_with(const std::string& str, const std::string& prefix) {
    if (str.length() < prefix.length()) {
        return false;
    }
    return str.substr(0, prefix.length()) == prefix;
}

bool ends_with(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.substr(str.length() - suffix.length()) == suffix;
}

} // namespace string

} // namespace utils
