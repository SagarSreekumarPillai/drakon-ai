#include "dataset.h"
#include "utils.h"
#include <iostream>
#include <cassert>

void test_synthetic_dataset() {
    std::cout << "Testing synthetic dataset..." << std::endl;
    
    // Test classification dataset
    SyntheticDataset class_dataset(100, 5, 3, "classification", true);
    assert(class_dataset.size() == 100);
    assert(class_dataset.input_dim() == 5);
    assert(class_dataset.output_dim() == 3);
    
    // Test regression dataset
    SyntheticDataset reg_dataset(50, 4, 1, "regression", false);
    assert(reg_dataset.size() == 50);
    assert(reg_dataset.input_dim() == 4);
    assert(reg_dataset.output_dim() == 1);
    
    // Test batch retrieval
    auto [data, labels] = class_dataset.get_batch(10);
    assert(data.rows() == 10);
    assert(data.cols() == 5);
    assert(labels.rows() == 10);
    assert(labels.cols() == 3);
    
    // Test that labels are one-hot encoded for classification
    for (size_t i = 0; i < labels.rows(); ++i) {
        float sum = 0.0f;
        for (size_t j = 0; j < labels.cols(); ++j) {
            sum += labels(i, j);
        }
        assert(std::abs(sum - 1.0f) < 1e-6);  // Should sum to 1
    }
    
    std::cout << "✅ Synthetic dataset tests passed" << std::endl;
}

void test_dataloader() {
    std::cout << "Testing dataloader..." << std::endl;
    
    auto dataset = std::make_shared<SyntheticDataset>(100, 3, 2, "classification", true);
    DataLoader loader(dataset, 16, true);
    
    assert(loader.num_batches() == 7);  // ceil(100/16) = 7
    
    // Test batch retrieval
    for (size_t i = 0; i < loader.num_batches(); ++i) {
        auto [data, labels] = loader.get_batch(i);
        
        if (i < 6) {
            assert(data.rows() == 16);
        } else {
            assert(data.rows() == 4);  // Last batch
        }
        assert(data.cols() == 3);
        assert(labels.cols() == 2);
    }
    
    // Test reset
    loader.reset();
    auto [data, labels] = loader.get_batch(0);
    assert(data.rows() == 16);
    
    std::cout << "✅ Dataloader tests passed" << std::endl;
}

void test_preprocessing() {
    std::cout << "Testing preprocessing functions..." << std::endl;
    
    // Create test data
    Tensor data({4, 3});
    data(0, 0) = 1.0f; data(0, 1) = 2.0f; data(0, 2) = 3.0f;
    data(1, 0) = 4.0f; data(1, 1) = 5.0f; data(1, 2) = 6.0f;
    data(2, 0) = 7.0f; data(2, 1) = 8.0f; data(2, 2) = 9.0f;
    data(3, 0) = 10.0f; data(3, 1) = 11.0f; data(3, 2) = 12.0f;
    
    // Test normalization
    Tensor normalized = preprocessing::normalize(data);
    float mean_val = utils::math::mean(normalized);
    float std_val = utils::math::stddev(normalized);
    assert(std::abs(mean_val) < 1e-6);  // Should be close to 0
    assert(std::abs(std_val - 1.0f) < 1e-6);  // Should be close to 1
    
    // Test standardization
    Tensor standardized = preprocessing::standardize(data);
    float min_val = utils::math::min(standardized);
    float max_val = utils::math::max(standardized);
    assert(std::abs(min_val) < 1e-6);  // Should be close to 0
    assert(std::abs(max_val - 1.0f) < 1e-6);  // Should be close to 1
    
    // Test one-hot encoding
    Tensor labels({4, 1});
    labels(0, 0) = 0.0f;
    labels(1, 0) = 1.0f;
    labels(2, 0) = 2.0f;
    labels(3, 0) = 0.0f;
    
    Tensor one_hot = preprocessing::one_hot_encode(labels, 3);
    assert(one_hot.rows() == 4);
    assert(one_hot.cols() == 3);
    assert(one_hot(0, 0) == 1.0f);
    assert(one_hot(1, 1) == 1.0f);
    assert(one_hot(2, 2) == 1.0f);
    assert(one_hot(3, 0) == 1.0f);
    
    // Test train-val split
    auto [train_data, train_labels] = std::make_pair(data, labels);
    auto split_result = preprocessing::train_val_split(train_data, train_labels, 0.75f);
    
    auto train_split = split_result.first;
    auto val_split = split_result.second;
    
    assert(train_split.first.rows() == 3);  // 75% of 4 = 3
    assert(val_split.first.rows() == 1);    // 25% of 4 = 1
    
    std::cout << "✅ Preprocessing tests passed" << std::endl;
}

void test_dataset_shuffling() {
    std::cout << "Testing dataset shuffling..." << std::endl;
    
    // Create dataset with shuffling enabled
    SyntheticDataset dataset(10, 2, 2, "classification", true);
    
    // Get first batch
    auto [data1, labels1] = dataset.get_batch(5);
    
    // Reset and get first batch again
    dataset.reset();
    auto [data2, labels2] = dataset.get_batch(5);
    
    // The data should be different due to shuffling
    bool data_different = false;
    for (size_t i = 0; i < data1.size(); ++i) {
        if (std::abs(data1.data()[i] - data2.data()[i]) > 1e-6) {
            data_different = true;
            break;
        }
    }
    assert(data_different);
    
    // Test with shuffling disabled
    SyntheticDataset dataset_no_shuffle(10, 2, 2, "classification", false);
    auto [data3, labels3] = dataset_no_shuffle.get_batch(5);
    
    dataset_no_shuffle.reset();
    auto [data4, labels4] = dataset_no_shuffle.get_batch(5);
    
    // The data should be the same
    bool data_same = true;
    for (size_t i = 0; i < data3.size(); ++i) {
        if (std::abs(data3.data()[i] - data4.data()[i]) > 1e-6) {
            data_same = false;
            break;
        }
    }
    assert(data_same);
    
    std::cout << "✅ Dataset shuffling tests passed" << std::endl;
}

void test_batch_iteration() {
    std::cout << "Testing batch iteration..." << std::endl;
    
    auto dataset = std::make_shared<SyntheticDataset>(25, 4, 2, "classification", true);
    DataLoader loader(dataset, 8, true);
    
    size_t total_samples = 0;
    size_t batch_count = 0;
    
    for (size_t i = 0; i < loader.num_batches(); ++i) {
        auto [data, labels] = loader.get_batch(i);
        total_samples += data.rows();
        batch_count++;
        
        assert(data.cols() == 4);
        assert(labels.cols() == 2);
    }
    
    assert(total_samples == 25);
    assert(batch_count == 4);  // ceil(25/8) = 4
    
    std::cout << "✅ Batch iteration tests passed" << std::endl;
}

void test_dataset_statistics() {
    std::cout << "Testing dataset statistics..." << std::endl;
    
    SyntheticDataset dataset(1000, 10, 5, "classification", true);
    
    // Test that we can iterate through the entire dataset
    size_t total_samples = 0;
    for (size_t i = 0; i < dataset.size(); i += 32) {
        auto [data, labels] = dataset.get_batch(32);
        total_samples += data.rows();
    }
    
    assert(total_samples == dataset.size());
    
    // Test dataset properties
    assert(dataset.input_dim() == 10);
    assert(dataset.output_dim() == 5);
    
    std::cout << "✅ Dataset statistics tests passed" << std::endl;
}

int main() {
    std::cout << "=== Running Dataset Tests ===" << std::endl;
    
    try {
        test_synthetic_dataset();
        test_dataloader();
        test_preprocessing();
        test_dataset_shuffling();
        test_batch_iteration();
        test_dataset_statistics();
        
        std::cout << "\n🎉 All dataset tests passed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
