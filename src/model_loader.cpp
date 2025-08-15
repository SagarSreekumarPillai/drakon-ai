#include "model_loader.h"
#include <iostream>
#include <fstream>

static DrakonWeights default_weights() {
    DrakonWeights w;
    w.mock_weights = {1.1f, 2.2f, 3.3f, 4.4f};
    return w;
}

DrakonWeights ModelLoader::load(const std::string& path) {
    std::cout << "[🔍] Loading model from: " << path << std::endl;

    std::ifstream fin(path, std::ios::binary);
    if (!fin) {
        std::cerr << "[⚠️] Could not open model file. Using default weights.\n";
        return default_weights();
    }

    // Read all bytes, interpret as floats (no header).
    fin.seekg(0, std::ios::end);
    std::streampos len = fin.tellg();
    fin.seekg(0, std::ios::beg);

    if (len <= 0) {
        std::cerr << "[⚠️] Empty model file. Using default weights.\n";
        return default_weights();
    }

    size_t count = static_cast<size_t>(len) / sizeof(float);
    if (count == 0) {
        std::cerr << "[⚠️] File not a multiple of sizeof(float). Using default weights.\n";
        return default_weights();
    }

    DrakonWeights weights;
    weights.mock_weights.resize(count);
    fin.read(reinterpret_cast<char*>(weights.mock_weights.data()),
             static_cast<std::streamsize>(count * sizeof(float)));

    if (!fin) {
        std::cerr << "[⚠️] Read error. Using default weights.\n";
        return default_weights();
    }

    std::cout << "[📦] Loaded " << weights.mock_weights.size() << " weights.\n";
    for (size_t i = 0; i < weights.mock_weights.size(); ++i) {
        std::cout << "    w[" << i << "] = " << weights.mock_weights[i] << "\n";
    }
    return weights;
}
