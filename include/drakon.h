// include/drakon.h

#ifndef DRAKON_H
#define DRAKON_H

#include <string>
#include "model_loader.h"

class DrakonModel {
public:
    DrakonModel(const std::string& model_path);

    float forward(int token);

private:
    DrakonWeights weights;
    class DrakonBackend* backend_;
};

#endif // DRAKON_H
