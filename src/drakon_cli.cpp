#include "drakon.h"
#include "tokenizer.h"
#include <iostream>

int run_drakon_cli(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <model_path> <prompt>\n";
        return 1;
    }

    std::string model_path = argv[1];
    std::string prompt = argv[2];

    std::cout << "[🚀] Loading model...\n";
    DrakonModel model(model_path);

    std::cout << "[✂️] Tokenizing prompt...\n";
    Tokenizer tokenizer;
    auto tokens = tokenizer.encode(prompt);
    std::cout << "[🔢] Encoded " << tokens.size() << " tokens.\n";

    std::cout << "[⚙️] Running inference...\n";
    for (int token : tokens) {
        auto probs = model.forward(token);
        std::cout << "[🔄] Token " << token << " → Probabilities: ";
        for (float p : probs) {
            std::cout << p << " ";
        }
        std::cout << "\n";
    }

    std::cout << "[✅] Done.\n";
    return 0;
}
