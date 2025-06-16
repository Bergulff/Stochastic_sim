//
// Created by Nicolai Bergulff on 12/06/2025.
//
#ifndef STOCHASTIC_VESSEL_H
#define STOCHASTIC_VESSEL_H
#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <random>
#include "species.h"
#include "reaction.h"
#include "symbol_table.h"

namespace Stochastic {
    // R4
    class Vessel {
        std::string name_;
        std::vector<std::shared_ptr<Species>> species_;
        std::vector<std::shared_ptr<Reaction>> reactions_;
        // R3
        SymbolTable<std::string, size_t> species_name_to_id_;
        size_t next_species_id_;
        size_t next_reaction_id_;
        size_t environment_id_;
        std::mt19937 rng_{};

    public:
        explicit Vessel(const std::string& name);

        std::shared_ptr<Species> add(const std::string& name, size_t initial_count);
        void add(const ReactionBuilder& builder);

        std::shared_ptr<Species> environment() const;
        std::shared_ptr<Species> get_species(const std::string& name) const;
        const std::vector<std::shared_ptr<Species>>& get_species() const;

        // R6
        void simulate(double end_time,
                      std::function<void(double, const std::vector<std::shared_ptr<Species>>&)> observer = nullptr);


        std::string reaction_to_string(size_t idx) const;
        // R2
        std::string to_string() const;
        std::string to_dot() const;

    private:
        std::shared_ptr<Species> add_species(const std::string& name, size_t initial_count);
        void add_reaction(const std::vector<std::pair<size_t, size_t>>& reactants,
                  const std::vector<std::pair<size_t, size_t>>& products,
                  double rate);
        void update_all_delays();
        std::shared_ptr<Reaction> find_next_reaction();
    };

} // namespace Stochastic

#endif // STOCHASTIC_VESSEL_H