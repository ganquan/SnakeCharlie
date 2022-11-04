#pragma once

#include "Matrix.h"
#include <memory>

namespace NN {
    class MatrixMath {
    public:
        static void multiply(const std::shared_ptr<Matrix> &a, const std::shared_ptr<Matrix> &b, const std::shared_ptr<Matrix> &c);
    };
} // namespace utils