//
// Created by Nicolai Bergulff on 12/06/2025.
//
#include "../include/Stochastic/stochastic.h"
#include <limits>
#include <algorithm>
#include <iostream>

namespace Stochastic {

// === ReactionBuilder === //
// R1 - Operator overloading for reaction syntax
ReactionBuilder::ReactionBuilder() : rate_(0.0), has_rate_(false) {}

ReactionBuilder::ReactionBuilder(size_t species_id)
    : ReactionBuilder() {
    reactant_ids_.emplace_back(species_id, 1);
}

ReactionBuilder::ReactionBuilder(const std::vector<size_t>& reactants)
    : rate_(0.0), has_rate_(false) {
    for (size_t id : reactants)
        reactant_ids_.emplace_back(id, 1);
}

// R1 - Chain reactions with + operator
ReactionBuilder ReactionBuilder::operator+(const Species& species) const {
    ReactionBuilder result(*this);
    result.reactant_ids_.emplace_back(species.id(), 1);
    return result;
}

// R1 - Add rate with >> operator
ReactionBuilder ReactionBuilder::operator>>(double rate) const {
    ReactionBuilder result(*this);
    result.rate_ = rate;
    result.has_rate_ = true;
    return result;
}

// R1 - Add products with >>= operator
ReactionBuilder ReactionBuilder::operator>>=(const Species& product) const {
    ReactionBuilder result(*this);
    result.product_ids_.emplace_back(product.id(), 1);
    return result;
}

ReactionBuilder ReactionBuilder::operator>>=(const ReactionBuilder& products) const {
    ReactionBuilder result(*this);
    result.product_ids_.insert(result.product_ids_.end(),
                               products.reactant_ids_.begin(),
                               products.reactant_ids_.end());
    return result;
}

const std::vector<std::pair<size_t, size_t>>& ReactionBuilder::reactants() const { return reactant_ids_; }
const std::vector<std::pair<size_t, size_t>>& ReactionBuilder::products() const { return product_ids_; }
double ReactionBuilder::rate() const { return rate_; }
bool ReactionBuilder::has_rate() const { return has_rate_; }

// === Reaction === //
// R4 - Reaction implementation for stochastic simulation
Reaction::Reaction(const std::vector<std::pair<size_t, size_t>>& reactants,
                   const std::vector<std::pair<size_t, size_t>>& products,
                   double rate, size_t id)
    : reactant_ids_(reactants), product_ids_(products),
      rate_(rate), current_delay_(std::numeric_limits<double>::infinity()), id_(id) {}

void Reaction::set_delay(double delay) { current_delay_ = delay; }

const std::vector<std::pair<size_t, size_t>>& Reaction::reactants() const { return reactant_ids_; }
const std::vector<std::pair<size_t, size_t>>& Reaction::products() const { return product_ids_; }
double Reaction::rate() const { return rate_; }
double Reaction::delay() const { return current_delay_; }
size_t Reaction::id() const { return id_; }

// R4 - Check if reaction can proceed based on available reactants
bool Reaction::can_proceed(const std::vector<std::shared_ptr<Species>>& species) const {
    for (const auto& [id, count] : reactant_ids_) {
        if (id >= species.size() || species[id]->count() < count) {
            return false;
        }
    }
    return true;
}

// R4 - Calculate reaction propensity (rate * reactant combinations)
double Reaction::calculate_propensity(const std::vector<std::shared_ptr<Species>>& species) const {
    double propensity = rate_;

    for (const auto& [id, count] : reactant_ids_) {
        if (id >= species.size()) return 0.0;

        size_t available = species[id]->count();
        if (available < count) return 0.0;

        // Calculate combinations: available * (available-1) * ... * (available-count+1)
        for (size_t i = 0; i < count; ++i) {
            propensity *= (available - i);
        }
    }

    return propensity;
}

// R4 - Execute reaction by updating species counts
void Reaction::execute(std::vector<std::shared_ptr<Species>>& species) const {
    if (!can_proceed(species)) {
        throw SimulationException("Cannot execute reaction: insufficient reactants");
    }

    // Consume reactants
    for (const auto& [id, count] : reactant_ids_) {
        species[id]->decrement_count(count);
    }

    // Produce products
    for (const auto& [id, count] : product_ids_) {
        species[id]->increment_count(count);
    }
}

// R4 - Comparator for priority queue (min-heap based on delay)
bool ReactionComparator::operator()(const std::shared_ptr<Reaction>& a, const std::shared_ptr<Reaction>& b) const {
    return a->delay() > b->delay(); // Min-heap: smaller delay has higher priority
}

} // namespace Stochastic