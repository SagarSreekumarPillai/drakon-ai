#include "model_loader.h"
#include <iostream>
#include <fstream>

DrakonWeights ModelLoader::load(const std::string& path) {
    std::cout << "[🔍] Loading model from: " << path << std::endl;

    std::ifstream in(path, std::ios::binary);
    if (!in) {
        std::cerr << "[❌] Could not open model file.\n";
        return {};
    }

    DrakonWeights weights;
    in.seekg(0, std::ios::end);
    size_t size = in.tellg();
    in.seekg(0, std::ios::beg);

    size_t count = size / sizeof(float);
    weights.mock_weights.resize(count);
    in.read(reinterpret_cast<char*>(weights.mock_weights.data()), size);

    std::cout << "[📦] Loaded " << count << " weights.\n";
    for (size_t i = 0; i < count; ++i) {
        std::cout << "    w[" << i << "] = " << weights.mock_weights[i] << "\n";
    }
    return weights;
}
