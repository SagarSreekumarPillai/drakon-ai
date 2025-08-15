#ifndef DATASET_H
#define DATASET_H

#include "tensor.h"
#include <vector>
#include <string>
#include <memory>
#include <random>

/**
 * @brief Base class for all datasets
 */
class Dataset {
protected:
    std::vector<Tensor> data;
    std::vector<Tensor> labels;
    size_t current_index;
    bool shuffle_enabled;
    std::mt19937 rng;
    
public:
    /**
     * @brief Constructor for dataset
     * @param shuffle Whether to shuffle the data
     */
    explicit Dataset(bool shuffle = true);
    virtual ~Dataset() = default;
    
    /**
     * @brief Get the size of the dataset
     * @return Number of samples
     */
    virtual size_t size() const = 0;
    
    /**
     * @brief Get the input dimension
     * @return Input feature dimension
     */
    virtual size_t input_dim() const = 0;
    
    /**
     * @brief Get the output dimension
     * @return Output label dimension
     */
    virtual size_t output_dim() const = 0;
    
    /**
     * @brief Get a batch of data
     * @param batch_size Size of the batch
     * @return Pair of (data, labels) tensors
     */
    virtual std::pair<Tensor, Tensor> get_batch(size_t batch_size);
    
    /**
     * @brief Reset the dataset iterator
     */
    virtual void reset();
    
    /**
     * @brief Enable or disable shuffling
     * @param shuffle Whether to shuffle
     */
    void set_shuffle(bool shuffle);
    
    /**
     * @brief Shuffle the dataset
     */
    void shuffle_data();
};

/**
 * @brief Synthetic dataset for testing
 */
class SyntheticDataset : public Dataset {
private:
    size_t num_samples;
    size_t input_features;
    size_t num_classes;
    std::string problem_type;  // "classification" or "regression"
    
public:
    /**
     * @brief Constructor for synthetic dataset
     * @param num_samples Number of samples to generate
     * @param input_features Number of input features
     * @param num_classes Number of output classes (for classification)
     * @param problem_type Type of problem ("classification" or "regression")
     * @param shuffle Whether to shuffle the data
     */
    SyntheticDataset(size_t num_samples, size_t input_features, size_t num_classes = 2,
                     const std::string& problem_type = "classification", bool shuffle = true);
    
    size_t size() const override;
    size_t input_dim() const override;
    size_t output_dim() const override;
    
    /**
     * @brief Generate synthetic data
     */
    void generate_data();
};

/**
 * @brief CSV dataset loader
 */
class CSVDataset : public Dataset {
private:
    std::string file_path;
    std::vector<size_t> feature_columns;
    size_t label_column;
    bool has_header;
    
public:
    /**
     * @brief Constructor for CSV dataset
     * @param file_path Path to the CSV file
     * @param feature_columns Indices of feature columns
     * @param label_column Index of label column
     * @param has_header Whether the CSV has a header row
     * @param shuffle Whether to shuffle the data
     */
    CSVDataset(const std::string& file_path, const std::vector<size_t>& feature_columns,
               size_t label_column, bool has_header = true, bool shuffle = true);
    
    size_t size() const override;
    size_t input_dim() const override;
    size_t output_dim() const override;
    
    /**
     * @brief Load data from CSV file
     * @return True if loading was successful
     */
    bool load_data();
};

/**
 * @brief Data preprocessing utilities
 */
namespace preprocessing {
    
    /**
     * @brief Normalize data to zero mean and unit variance
     * @param data Input data tensor
     * @return Normalized data tensor
     */
    Tensor normalize(const Tensor& data);
    
    /**
     * @brief Standardize data using min-max scaling
     * @param data Input data tensor
     * @return Standardized data tensor
     */
    Tensor standardize(const Tensor& data);
    
    /**
     * @brief One-hot encode labels
     * @param labels Input labels (integer indices)
     * @param num_classes Number of classes
     * @return One-hot encoded labels
     */
    Tensor one_hot_encode(const Tensor& labels, size_t num_classes);
    
    /**
     * @brief Split dataset into train and validation sets
     * @param data Input data
     * @param labels Input labels
     * @param train_ratio Ratio of training data (0.0 to 1.0)
     * @return Pair of (train_data, train_labels) and (val_data, val_labels)
     */
    std::pair<std::pair<Tensor, Tensor>, std::pair<Tensor, Tensor>> 
    train_val_split(const Tensor& data, const Tensor& labels, float train_ratio = 0.8f);
    
} // namespace preprocessing

/**
 * @brief DataLoader for batching and iteration
 */
class DataLoader {
private:
    std::shared_ptr<Dataset> dataset;
    size_t batch_size;
    bool shuffle;
    
public:
    /**
     * @brief Constructor for DataLoader
     * @param dataset Dataset to load from
     * @param batch_size Size of batches
     * @param shuffle Whether to shuffle the data
     */
    DataLoader(std::shared_ptr<Dataset> dataset, size_t batch_size, bool shuffle = true);
    
    /**
     * @brief Get the number of batches
     * @return Number of batches
     */
    size_t num_batches() const;
    
    /**
     * @brief Get a batch
     * @param batch_idx Batch index
     * @return Pair of (data, labels) tensors
     */
    std::pair<Tensor, Tensor> get_batch(size_t batch_idx);
    
    /**
     * @brief Reset the dataloader
     */
    void reset();
};

#endif // DATASET_H
