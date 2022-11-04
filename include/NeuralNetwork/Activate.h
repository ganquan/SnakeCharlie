#pragma once

#include <functional>
#include <map>

typedef std::function<double(double)> ActivateFunction;

namespace NN {

class ActivationType {
public:
    enum Type {
        none,
        relu,
        sigmoid
    };

    ActivationType() = default;
    ~ActivationType() = default;
    constexpr ActivationType(Type atype) : type(atype) {}

    // Allow switch and comparisons.
    constexpr operator Type() const { return type; }
    // Prevent usage: if(mode)
    explicit operator bool() const = delete;

private:
    Type type;
};

class Activation {
public:
    static std::map<ActivationType, ActivateFunction> ActivateMap;
    static std::map<ActivationType, ActivateFunction> DeriveMap;
};

}; // namespace NN
