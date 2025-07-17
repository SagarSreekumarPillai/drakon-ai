// Path: include/backend.h

#ifndef DRAKON_BACKEND_H
#define DRAKON_BACKEND_H

#include <vector>

class Backend {
public:
    Backend();
    void run(const std::vector<float>& input);
    std::vector<float> get_output();

private:
    std::vector<float> output_data;
};

#endif // DRAKON_BACKEND_H
