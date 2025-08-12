#include "model_loader.h"
#include <iostream>
#include <fstream>

DrakonWeights ModelLoader::load(const std::string& path) {
    std::cout << "[🔍] Loading model from: " << path << std::endl;

    DrakonWeights weights;

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "[❌] Failed to open model file: " << path << std::endl;
        return weights; // return empty weights
    }

    // Read number of floats in file
    file.seekg(0, std::ios::end);
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    size_t num_floats = file_size / sizeof(float);
    weights.mock_weights.resize(num_floats);

    file.read(reinterpret_cast<char*>(weights.mock_weights.data()),
              num_floats * sizeof(float));

    std::cout << "[📦] Loaded " << num_floats << " weights." << std::endl;
    return weights;
}
