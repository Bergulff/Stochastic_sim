//
// Created by Nicolai Bergulff on 12/06/2025.
//
#include "../include/Stochastic/stochastic.h"
#include "../include/Stochastic/vessel.h"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <random>
#include <limits>

namespace Stochastic {

// R4
Vessel::Vessel(const std::string& name)
    : name_(name), next_species_id_(0), next_reaction_id_(0), rng_(std::random_device{}()) {
    environment_id_ = add_species("environment", 0)->id();
}

// R3
std::shared_ptr<Species> Vessel::add(const std::string& name, size_t initial_count) {
    return add_species(name, initial_count);
}

std::shared_ptr<Species> Vessel::environment() const {
    return species_[environment_id_];
}

// R4
void Vessel::add(const ReactionBuilder& builder) {
    if (!builder.has_rate())
        throw SimulationException("Reaction must have a rate specified");
    add_reaction(builder.reactants(), builder.products(), builder.rate());
}

// R3
std::shared_ptr<Species> Vessel::add_species(const std::string& name, size_t initial_count) {
    if (species_name_to_id_.contains(name))
        throw SymbolTableException("Species already exists: " + name);
    auto sp = std::make_shared<Species>(name, initial_count, next_species_id_);
    species_.push_back(sp);
    species_name_to_id_.add(name, next_species_id_);
    return species_[next_species_id_++];
}

// R4
void Vessel::add_reaction(const std::vector<std::pair<size_t, size_t>>& reactants,
                         const std::vector<std::pair<size_t, size_t>>& products,
                         double rate) {
    auto r = std::make_shared<Reaction>(reactants, products, rate, next_reaction_id_++);
    reactions_.push_back(r);
}

// R4
std::shared_ptr<Reaction> Vessel::find_next_reaction() {
    auto min_it = std::min_element(reactions_.begin(), reactions_.end(),
        [](const auto& a, const auto& b) { return a->delay() < b->delay(); });
    return *min_it;
}

// R4
void Vessel::update_all_delays() {
    for (auto& r : reactions_) {
        double propensity = r->calculate_propensity(species_);
        if (propensity > 0) {
            std::exponential_distribution<double> exp_dist(propensity);
            r->set_delay(exp_dist(rng_));
        } else {
            r->set_delay(std::numeric_limits<double>::infinity());
        }
    }
}

// R4 & R6
    void Vessel::simulate(double end_time, std::function<void(double, const std::vector<std::shared_ptr<Species>>&)> observer) {
    double time = 0.0;

    if (observer) observer(time, species_);

    while (time < end_time) {
        update_all_delays();
        auto next = find_next_reaction();

        double dt = next->delay();
        if (dt == std::numeric_limits<double>::infinity() || !std::isfinite(dt)) {
            break;
        }

        time += dt;
        if (time >= end_time) break;

        try {
            if (next->can_proceed(species_)) {
                next->execute(species_);
                if (observer) observer(time, species_);
            } else {
                break;
            }
        } catch (const SimulationException& e) {
            break;
        }
    }
}

// R3
std::shared_ptr<Species> Vessel::get_species(const std::string& name) const {
    return species_[species_name_to_id_.get(name)];
}

const std::vector<std::shared_ptr<Species>>& Vessel::get_species() const {
    return species_;
}

// R2
std::string Vessel::reaction_to_string(size_t idx) const {
    if (idx >= reactions_.size()) return "Invalid reaction index";

    std::stringstream ss;
    auto& r = reactions_[idx];

    // Reactants
    bool first = true;
    for (const auto& [id, count] : r->reactants()) {
        if (!first) ss << " + ";
        if (count > 1) ss << count << "*";
        ss << species_[id]->name();
        first = false;
    }

    ss << " --(" << r->rate() << ")--> ";

    // Products
    first = true;
    for (const auto& [id, count] : r->products()) {
        if (!first) ss << " + ";
        if (count > 1) ss << count << "*";
        ss << species_[id]->name();
        first = false;
    }

    return ss.str();
}

// R2 - Human readable system state
std::string Vessel::to_string() const {
    std::stringstream ss;
    ss << "Vessel: " << name_ << "\n";
    ss << "Species:\n";
    for (const auto& s : species_) {
        ss << "  " << s->name() << " = " << s->count() << "\n";
    }
    ss << "Reactions:\n";
    for (size_t i = 0; i < reactions_.size(); ++i) {
        ss << "  " << i << ": " << reaction_to_string(i) << "\n";
    }
    return ss.str();
}

// R2 - DOT graph format for visualization
std::string Vessel::to_dot() const {
    std::stringstream ss;
    ss << "digraph \"" << name_ << "\" {\n";
    ss << "  rankdir=LR;\n";

    // Species nodes
    for (const auto& s : species_) {
        if (s->name() == "environment") continue; // Skip environment
        ss << "  s" << s->id() << "[label=\"" << s->name() << "\\n(" << s->count()
           << ")\",shape=box,style=filled,fillcolor=cyan];\n";
    }

    // Reaction nodes
    for (const auto& r : reactions_) {
        ss << "  r" << r->id() << "[label=\"" << r->rate()
           << "\",shape=oval,style=filled,fillcolor=yellow];\n";

        // Edges from reactants to reaction
        for (const auto& [id, count] : r->reactants()) {
            if (species_[id]->name() == "environment") continue;
            ss << "  s" << id << " -> r" << r->id();
            if (count > 1) ss << "[label=\"" << count << "\"]";
            ss << ";\n";
        }

        // Edges from reaction to products
        for (const auto& [id, count] : r->products()) {
            if (species_[id]->name() == "environment") continue;
            ss << "  r" << r->id() << " -> s" << id;
            if (count > 1) ss << "[label=\"" << count << "\"]";
            ss << ";\n";
        }
    }

    ss << "}\n";
    return ss.str();
}

} // namespace Stochastic