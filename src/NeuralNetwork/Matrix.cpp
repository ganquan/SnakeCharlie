#include "NeuralNetwork/Matrix.h"

#include <iostream>
#include <random>

Matrix::Matrix(int row, int col, bool isRandom) {
    this->m_rowNum = row;
    this->m_colNum = col;

    for (int i = 0; i < m_rowNum; i++) {
        std::vector<double> colValues;
        for (int j = 0; j < m_colNum; j++) {
            double r = 0.00;

            if (isRandom) {
                r = this->getRandomNumber();
            }

            colValues.push_back(r);
        }

        this->m_values.push_back(colValues);
    }
}

Matrix::~Matrix() {}

void Matrix::fillWithRandom() {
    for (int i = 0; i < m_rowNum; i++) {
        for (int j = 0; j < m_colNum; j++) {
            double r = this->getRandomNumber();
            this->setValue(i, j, r);
        }
    }
}

void Matrix::setValue(int r, int c, double v) { m_values.at(r).at(c) = v; }
double Matrix::getValue(int r, int c) { return m_values.at(r).at(c); }

double Matrix::getRandomNumber() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0, 1);

    return dist(gen);
}
