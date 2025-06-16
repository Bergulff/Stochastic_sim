//
// Created by Nicolai Bergulff on 12/06/2025.
//
#ifndef STOCHASTIC_SPECIES_H
#define STOCHASTIC_SPECIES_H
#include <string>

namespace Stochastic {
    class ReactionBuilder;

    class Species {
        std::string name_;
        size_t count_;
        size_t id_;
    public:
        Species(const std::string& name, size_t initial_count, size_t id);

        const std::string& name() const;
        size_t count() const;
        size_t id() const;

        void set_count(size_t count);
        void increment_count(size_t amount = 1);
        void decrement_count(size_t amount = 1);

        // R1
        ReactionBuilder operator+(const Species& other) const;
        ReactionBuilder operator>>(double rate) const;
        ReactionBuilder operator>>=(const Species& product) const;
    };
}
#endif // STOCHASTIC_SPECIES_H
