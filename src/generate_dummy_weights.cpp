#include <iostream>
#include <fstream>
#include <vector>
#include <random>

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <output_path> <rows> <cols>\n";
        return 1;
    }

    std::string out_path = argv[1];
    int rows = std::stoi(argv[2]);
    int cols = std::stoi(argv[3]);
    int total = rows * cols;

    std::vector<float> weights(total);
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (float &w : weights) {
        w = dist(rng);
    }

    std::ofstream out(out_path, std::ios::binary);
    out.write(reinterpret_cast<const char*>(weights.data()), total * sizeof(float));

    std::cout << "[💾] Wrote " << total << " random weights to " << out_path << "\n";
    return 0;
}
