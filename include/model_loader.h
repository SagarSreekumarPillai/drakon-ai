// include/model_loader.h

#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <string>
#include <vector>

struct DrakonWeights {
    // For now, use a dummy vector of floats. Expand later.
    std::vector<float> mock_weights;
};

class ModelLoader {
public:
    static DrakonWeights load(const std::string& path);
};

#endif // MODEL_LOADER_H
