// src/main.cpp

#include "drakon.h" 
#include "drakon_cli.h"
#include <iostream>
#include <vector>
#include <string>


int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <model-path> <prompt>" << std::endl;
        return 1;
    }

    return run_drakon_cli(argc, argv);
}