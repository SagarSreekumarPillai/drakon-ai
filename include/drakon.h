#ifndef DRAKON_H
#define DRAKON_H

#include <string>
#include <vector>
#include "model_loader.h"
#include <unordered_map>    

struct DrakonWeights; // forward-declare (defined in model_loader.h)

class DrakonModel {
public:
    explicit DrakonModel(const std::string& model_path);

    // Single-token forward (existing behavior)
    float forward(int token);

    // NEW: Batch forward
    std::vector<float> forward_batch(const std::vector<int>& tokens);

    // NEW: Optional caching control
    void enable_cache(bool on) { use_cache_ = on; }
    void clear_cache() { cache_.clear(); }

private:
    DrakonWeights weights;
    bool use_cache_ = true;
    std::unordered_map<int, float> cache_;
};

#endif // DRAKON_H
