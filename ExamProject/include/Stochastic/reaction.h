//
// Created by Nicolai Bergulff on 12/06/2025.
//
#ifndef STOCHASTIC_REACTION_H
#define STOCHASTIC_REACTION_H

#include <vector>
#include <memory>

namespace Stochastic {
    class Species;

    class ReactionBuilder {
        std::vector<std::pair<size_t, size_t>> reactant_ids_;
        std::vector<std::pair<size_t, size_t>> product_ids_;
        double rate_;
        bool has_rate_;
    public:
        ReactionBuilder();
        ReactionBuilder(size_t species_id);
        ReactionBuilder(const std::vector<size_t>& reactants);

        ReactionBuilder operator+(const Species& species) const;
        ReactionBuilder operator>>(double rate) const;
        ReactionBuilder operator>>=(const Species& product) const;
        ReactionBuilder operator>>=(const ReactionBuilder& products) const;

        const std::vector<std::pair<size_t, size_t>>& reactants() const;
        const std::vector<std::pair<size_t, size_t>>& products() const;
        double rate() const;
        bool has_rate() const;
    };

    class Reaction {
        std::vector<std::pair<size_t, size_t>> reactant_ids_;
        std::vector<std::pair<size_t, size_t>> product_ids_;
        double rate_;
        double current_delay_;
        size_t id_;
    public:
        Reaction(const std::vector<std::pair<size_t, size_t>>& reactants,
                 const std::vector<std::pair<size_t, size_t>>& products,
                 double rate, size_t id);

        const std::vector<std::pair<size_t, size_t>>& reactants() const;
        const std::vector<std::pair<size_t, size_t>>& products() const;
        double rate() const;
        double delay() const;
        size_t id() const;

        void set_delay(double delay);
        bool can_proceed(const std::vector<std::shared_ptr<Species>>& species) const;
        double calculate_propensity(const std::vector<std::shared_ptr<Species>>& species) const;
        void execute(std::vector<std::shared_ptr<Species>>& species) const;
    };

    struct ReactionComparator {
        bool operator()(const std::shared_ptr<Reaction>& a, const std::shared_ptr<Reaction>& b) const;
    };
}

#endif // STOCHASTIC_REACTION_H

