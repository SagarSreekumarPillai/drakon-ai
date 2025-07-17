// include/backend.h

#ifndef BACKEND_H
#define BACKEND_H

#include <vector>

class DrakonBackend {
public:
    DrakonBackend(const std::vector<float>& weights);

    float forward(int token);

private:
    std::vector<float> weights_;
};

#endif // BACKEND_H
