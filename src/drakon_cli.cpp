#include <iostream>
#include <string>
#include "drakon.h"
#include "drakon_cli.h"
#include "tokenizer.h"

int run_drakon_cli(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "[❌] Usage: drakon <model_path> <prompt>\n";
        return 1;
    }

    std::string model_path = argv[1];
    std::string prompt = argv[2];

    std::cout << "[🚀] Loading model...\n";
    DrakonModel model(model_path);

    std::cout << "[✂️] Tokenizing prompt...\n";
    Tokenizer tokenizer;
    std::vector<int> tokens = tokenizer.encode(prompt);

    std::cout << "[⚙️] Running inference...\n";
    for (int token : tokens) {
        float output = model.forward(token);
        std::cout << "[🔄] Token " << token << " → Output: " << output << "\n";
    }

    std::cout << "[✅] Done.\n";
    return 0;
}
