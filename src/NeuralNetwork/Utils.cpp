#include "NeuralNetwork/Utils.h"


void NN::MatrixMath::multiply(const std::shared_ptr<Matrix> &a, const std::shared_ptr<Matrix> &b, const std::shared_ptr<Matrix> &c) {
    for (int i = 0; i < a->getRowNum(); i++) {
        for (int j = 0; j < b->getColNum(); j++) {
            for (int k = 0; k < b->getRowNum(); k++) {
                double p = a->getValue(i, k) * b->getValue(k, j);
                double newVal = c->getValue(i, j) + p;
                c->setValue(i, j, newVal);
            }

            c->setValue(i, j, c->getValue(i, j));
        }
    }
}