// include/drakon.h

#ifndef DRAKON_H
#define DRAKON_H

#include <string>

class DrakonModel {
public:
    DrakonModel(const std::string& model_path);

    float forward(int token);

private:
    class DrakonBackend* backend_;
};

#endif // DRAKON_H
