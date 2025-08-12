#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: generate_dummy_weights <output_file>\n";
        return 1;
    }

    std::string filename = argv[1];
    std::vector<float> weights = {1.1f, 2.2f, 3.3f, 4.4f}; // test values

    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        std::cerr << "Error opening file for writing: " << filename << "\n";
        return 1;
    }

    out.write(reinterpret_cast<const char*>(weights.data()),
              weights.size() * sizeof(float));

    out.close();
    std::cout << "[💾] Wrote " << weights.size()
              << " dummy weights to " << filename << "\n";
    return 0;
}
