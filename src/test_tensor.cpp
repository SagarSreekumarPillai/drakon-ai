#include "tensor.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "[TEST] Tensor Class\n";

    // 1. Test default constructor
    Tensor t0;
    assert(t0.rows() == 0 && t0.cols() == 0 && "Default tensor should be 0x0");

    // 2. Test shape-only constructor
    Tensor t1({2, 3});
    assert(t1.rows() == 2 && t1.cols() == 3);
    assert(t1.size() == 6);

    // 3. Test fill constructor
    Tensor t2({2, 2}, 5.0f);
    for (auto v : t2.data()) assert(v == 5.0f);

    // 4. Test operator() write & read
    t1(0, 0) = 1.1f;
    t1(0, 1) = 2.2f;
    t1(0, 2) = 3.3f;
    t1(1, 0) = 4.4f;
    t1(1, 1) = 5.5f;
    t1(1, 2) = 6.6f;

    assert(t1(1, 2) == 6.6f);

    // 5. Test getShape()
    auto shape = t1.getShape();
    assert(shape.size() == 2 && shape[0] == 2 && shape[1] == 3);

    // 6. Test print
    t1.print("t1 values");
    t2.print("t2 values");

    std::cout << "[PASS] All Tensor tests passed.\n";
    return 0;
}
