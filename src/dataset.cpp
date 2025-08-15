#include "dataset.h"
#include "utils.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

// Base Dataset implementation
Dataset::Dataset(bool shuffle) : current_index(0), shuffle_enabled(shuffle) {
    std::random_device rd;
    rng.seed(rd());
}

std::pair<Tensor, Tensor> Dataset::get_batch(size_t batch_size) {
    if (data.empty() || labels.empty()) {
        throw std::runtime_error("Dataset is empty");
    }
    
    size_t actual_batch_size = std::min(batch_size, data.size() - current_index);
    
    // Create batch tensors
    std::vector<size_t> data_shape = data[0].shape();
    data_shape[0] = actual_batch_size;
    Tensor batch_data(data_shape);
    
    std::vector<size_t> label_shape = labels[0].shape();
    label_shape[0] = actual_batch_size;
    Tensor batch_labels(label_shape);
    
    // Copy data to batch
    for (size_t i = 0; i < actual_batch_size; ++i) {
        size_t idx = (current_index + i) % data.size();
        // For simplicity, assume 2D tensors (batch_size, features)
        for (size_t j = 0; j < data[idx].cols(); ++j) {
            batch_data(i, j) = data[idx](0, j);
        }
        for (size_t j = 0; j < labels[idx].cols(); ++j) {
            batch_labels(i, j) = labels[idx](0, j);
        }
    }
    
    current_index += actual_batch_size;
    if (current_index >= data.size()) {
        current_index = 0;
        if (shuffle_enabled) {
            shuffle_data();
        }
    }
    
    return {batch_data, batch_labels};
}

void Dataset::reset() {
    current_index = 0;
    if (shuffle_enabled) {
        shuffle_data();
    }
}

void Dataset::set_shuffle(bool shuffle) {
    shuffle_enabled = shuffle;
}

void Dataset::shuffle_data() {
    if (data.size() != labels.size()) {
        throw std::runtime_error("Data and labels have different sizes");
    }
    
    std::vector<size_t> indices(data.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        indices[i] = i;
    }
    
    std::shuffle(indices.begin(), indices.end(), rng);
    
    std::vector<Tensor> shuffled_data = data;
    std::vector<Tensor> shuffled_labels = labels;
    
    for (size_t i = 0; i < indices.size(); ++i) {
        data[i] = shuffled_data[indices[i]];
        labels[i] = shuffled_labels[indices[i]];
    }
}

// SyntheticDataset implementation
SyntheticDataset::SyntheticDataset(size_t num_samples, size_t input_features, size_t num_classes,
                                   const std::string& problem_type, bool shuffle)
    : Dataset(shuffle), num_samples(num_samples), input_features(input_features),
      num_classes(num_classes), problem_type(problem_type) {
    generate_data();
}

size_t SyntheticDataset::size() const {
    return num_samples;
}

size_t SyntheticDataset::input_dim() const {
    return input_features;
}

size_t SyntheticDataset::output_dim() const {
    return num_classes;
}

void SyntheticDataset::generate_data() {
    data.clear();
    labels.clear();
    
    std::mt19937& rng = utils::random::get_rng();
    std::normal_distribution<float> feature_dist(0.0f, 1.0f);
    
    for (size_t i = 0; i < num_samples; ++i) {
        // Generate random features
        Tensor sample({1, static_cast<int>(input_features)});
        for (size_t j = 0; j < input_features; ++j) {
            sample(0, j) = feature_dist(rng);
        }
        data.push_back(sample);
        
        // Generate labels
        if (problem_type == "classification") {
            // For classification, create one-hot encoded labels
            Tensor label({1, static_cast<int>(num_classes)}, 0.0f);
            int class_idx = std::uniform_int_distribution<int>(0, num_classes - 1)(rng);
            label(0, class_idx) = 1.0f;
            labels.push_back(label);
        } else if (problem_type == "regression") {
            // For regression, create a simple linear combination of features
            Tensor label({1, 1});
            float target = 0.0f;
            for (size_t j = 0; j < input_features; ++j) {
                target += sample(0, j) * (j + 1) * 0.1f;  // Simple linear function
            }
            target += std::normal_distribution<float>(0.0f, 0.1f)(rng);  // Add noise
            label(0, 0) = target;
            labels.push_back(label);
        }
    }
}

// CSVDataset implementation
CSVDataset::CSVDataset(const std::string& file_path, const std::vector<size_t>& feature_columns,
                       size_t label_column, bool has_header, bool shuffle)
    : Dataset(shuffle), file_path(file_path), feature_columns(feature_columns),
      label_column(label_column), has_header(has_header) {
    load_data();
}

size_t CSVDataset::size() const {
    return data.size();
}

size_t CSVDataset::input_dim() const {
    return feature_columns.size();
}

size_t CSVDataset::output_dim() const {
    return 1;  // Assuming single label column
}

bool CSVDataset::load_data() {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + file_path);
    }
    
    std::string line;
    size_t line_count = 0;
    
    while (std::getline(file, line)) {
        if (has_header && line_count == 0) {
            line_count++;
            continue;
        }
        
        std::vector<std::string> tokens = utils::string::split(line, ',');
        if (tokens.size() <= std::max(*std::max_element(feature_columns.begin(), feature_columns.end()), label_column)) {
            continue;  // Skip malformed lines
        }
        
        // Extract features
        Tensor sample({1, static_cast<int>(feature_columns.size())});
        for (size_t i = 0; i < feature_columns.size(); ++i) {
            try {
                sample(0, i) = std::stof(utils::string::trim(tokens[feature_columns[i]]));
            } catch (const std::exception&) {
                sample(0, i) = 0.0f;  // Default value for invalid numbers
            }
        }
        data.push_back(sample);
        
        // Extract label
        Tensor label({1, 1});
        try {
            label(0, 0) = std::stof(utils::string::trim(tokens[label_column]));
        } catch (const std::exception&) {
            label(0, 0) = 0.0f;  // Default value for invalid numbers
        }
        labels.push_back(label);
        
        line_count++;
    }
    
    return !data.empty();
}

// DataLoader implementation
DataLoader::DataLoader(std::shared_ptr<Dataset> dataset, size_t batch_size, bool shuffle)
    : dataset(dataset), batch_size(batch_size), shuffle(shuffle) {}

size_t DataLoader::num_batches() const {
    return (dataset->size() + batch_size - 1) / batch_size;
}

std::pair<Tensor, Tensor> DataLoader::get_batch(size_t batch_idx) {
    if (batch_idx >= num_batches()) {
        throw std::out_of_range("Batch index out of range");
    }
    
    return dataset->get_batch(batch_size);
}

void DataLoader::reset() {
    dataset->reset();
}

// Preprocessing utilities
namespace preprocessing {

Tensor normalize(const Tensor& data) {
    float mean_val = utils::math::mean(data);
    float std_val = utils::math::stddev(data);
    
    Tensor result = data;
    for (float& val : result.data()) {
        val = (val - mean_val) / std_val;
    }
    return result;
}

Tensor standardize(const Tensor& data) {
    float min_val = utils::math::min(data);
    float max_val = utils::math::max(data);
    float range = max_val - min_val;
    
    if (range == 0.0f) {
        return Tensor(data.shape(), 0.0f);
    }
    
    Tensor result = data;
    for (float& val : result.data()) {
        val = (val - min_val) / range;
    }
    return result;
}

Tensor one_hot_encode(const Tensor& labels, size_t num_classes) {
    if (labels.dims() != 2) {
        throw std::invalid_argument("Labels must be 2D tensor");
    }
    
    Tensor result({static_cast<int>(labels.rows()), static_cast<int>(num_classes)}, 0.0f);
    
    for (int i = 0; i < labels.rows(); ++i) {
        int class_idx = static_cast<int>(labels(i, 0));
        if (class_idx >= 0 && class_idx < static_cast<int>(num_classes)) {
            result(i, class_idx) = 1.0f;
        }
    }
    
    return result;
}

std::pair<std::pair<Tensor, Tensor>, std::pair<Tensor, Tensor>> 
train_val_split(const Tensor& data, const Tensor& labels, float train_ratio) {
    if (data.rows() != labels.rows()) {
        throw std::invalid_argument("Data and labels must have same number of rows");
    }
    
    size_t total_samples = data.rows();
    size_t train_samples = static_cast<size_t>(total_samples * train_ratio);
    
    // Create train data
    Tensor train_data({static_cast<int>(train_samples), data.cols()});
    Tensor train_labels({static_cast<int>(train_samples), labels.cols()});
    
    // Create validation data
    Tensor val_data({static_cast<int>(total_samples - train_samples), data.cols()});
    Tensor val_labels({static_cast<int>(total_samples - train_samples), labels.cols()});
    
    // Copy data
    for (int i = 0; i < static_cast<int>(train_samples); ++i) {
        for (int j = 0; j < data.cols(); ++j) {
            train_data(i, j) = data(i, j);
        }
        for (int j = 0; j < labels.cols(); ++j) {
            train_labels(i, j) = labels(i, j);
        }
    }
    
    for (int i = 0; i < static_cast<int>(total_samples - train_samples); ++i) {
        for (int j = 0; j < data.cols(); ++j) {
            val_data(i, j) = data(static_cast<int>(train_samples) + i, j);
        }
        for (int j = 0; j < labels.cols(); ++j) {
            val_labels(i, j) = labels(static_cast<int>(train_samples) + i, j);
        }
    }
    
    return {{train_data, train_labels}, {val_data, val_labels}};
}

} // namespace preprocessing
