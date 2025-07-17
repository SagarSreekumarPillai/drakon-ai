// Path: include/model_loader.h

#ifndef DRAKON_MODEL_LOADER_H
#define DRAKON_MODEL_LOADER_H

#include <string>
#include <vector>

class ModelLoader {
public:
    explicit ModelLoader(const std::string& path);

    void load();
    std::vector<float> forward(const std::vector<int>& input_tokens);

private:
    std::string model_path;
    // Add other internal state as needed
};

#endif // DRAKON_MODEL_LOADER_H
