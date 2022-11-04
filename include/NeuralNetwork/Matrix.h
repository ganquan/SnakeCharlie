#pragma once

#include <vector>

class Matrix {
public:
    Matrix(int row, int col, bool isRandom);
    ~Matrix();

    void setValue(int r, int c, double v);
    double getValue(int r, int c);
    void fillWithRandom();

    int getRowNum() { return m_rowNum; }
    int getColNum() { return m_colNum; }

    std::vector<std::vector<double>> &getValues() { return this->m_values; }

private:
    double getRandomNumber();

private:
    int m_rowNum;
    int m_colNum;

    std::vector<std::vector<double>> m_values;
};
