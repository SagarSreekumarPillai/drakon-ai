#include "tensor.h"
#include "ops.h"

int main() {
    Tensor A({2, 3}, 1.0f); // All ones
    Tensor B({3, 2}, 2.0f); // All twos

    Tensor C = ops::matmul(A, B);
    C.print("A x B");

    Tensor activated = ops::relu(C);
    activated.print("ReLU(AxB)");

    Tensor sm = ops::softmax(C);
    sm.print("Softmax(AxB)");

    return 0;
}
