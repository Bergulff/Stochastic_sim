//
// Created by Nicolai Bergulff on 12/06/2025.
//
#include "../include/Stochastic/stochastic.h"
#include <stdexcept>

namespace Stochastic {

    Species::Species(const std::string& name, size_t initial_count, size_t id)
        : name_(name), count_(initial_count), id_(id) {}

    const std::string& Species::name() const { return name_; }
    size_t Species::count() const { return count_; }
    size_t Species::id() const { return id_; }

    void Species::set_count(size_t count) { count_ = count; }
    void Species::increment_count(size_t amount) { count_ += amount; }
    // R4
    void Species::decrement_count(size_t amount) {
        if (count_ < amount)
            throw SimulationException("Cannot decrement species " + name_ + " below zero");
        count_ -= amount;
    }
    // R1
    ReactionBuilder Species::operator+(const Species& other) const {
        return ReactionBuilder({id_, other.id_});
    }
    ReactionBuilder Species::operator>>(double rate) const {
        ReactionBuilder builder(id_);
        return builder >> rate;
    }
    ReactionBuilder Species::operator>>=(const Species& product) const {
        ReactionBuilder builder(id_);
        return (builder >> 1.0) >>= product;
    }

} // namespace Stochastic

