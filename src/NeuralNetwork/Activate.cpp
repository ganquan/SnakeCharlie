#include "NeuralNetwork/Activate.h"

#include <algorithm>
#include <cmath>
#include <map>

ActivateFunction sigmoid = [](double v) { return v / (1.0 + abs(v)); };
ActivateFunction sigmoidDerived = [](double v) { return v * (1.0 - v); };

ActivateFunction relu = [](double v) { return std::max(0.0, v); };
ActivateFunction reluDerived = [](double v) {

#if 0
    if (v < 0)
        return 0;

    if (v > 0)
        return 1;
#else
    // relu'(x) 在x == 0时 其实是undefine
    // 这里先处理成 x <= 0 return 0 以避免编译告警
    // 暂时没有用到的地方
    if (v <= 0) {
        return 0;
    } else {
        return 1;
    }
#endif
};

std::map<NN::ActivationType, ActivateFunction> NN::Activation::ActivateMap{

    {NN::ActivationType::none, nullptr},
    {NN::ActivationType::relu, relu},
    {NN::ActivationType::sigmoid, sigmoid}

};

std::map<NN::ActivationType, ActivateFunction> NN::Activation::DeriveMap{

    {NN::ActivationType::none, nullptr},
    {NN::ActivationType::relu, reluDerived},
    {NN::ActivationType::sigmoid, sigmoidDerived}

};
