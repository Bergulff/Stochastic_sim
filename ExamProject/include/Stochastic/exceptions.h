//
// Created by Nicolai Bergulff on 12/06/2025.
//
#ifndef STOCHASTIC_EXCEPTIONS_H
#define STOCHASTIC_EXCEPTIONS_H
#include <stdexcept>
#include <string>

namespace Stochastic {
    class SymbolTableException : public std::runtime_error {
    public:
        explicit SymbolTableException(const std::string& msg) : std::runtime_error(msg) {}
    };

    class SimulationException : public std::runtime_error {
    public:
        explicit SimulationException(const std::string& msg) : std::runtime_error(msg) {}
    };
}
#endif // STOCHASTIC_EXCEPTIONS_H
