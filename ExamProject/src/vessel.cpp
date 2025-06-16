//
// Created by Nicolai Bergulff on 12/06/2025.
//
#include "../include/Stochastic/stochastic.h"
#include "../include/Stochastic/vessel.h"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <random>

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
    return *std::min_element(reactions_.begin(), reactions_.end(), ReactionComparator());
}
// R4
void Vessel::update_all_delays() {
    for (auto& r : reactions_) {
        double p = r->calculate_propensity(species_);
        r->set_delay(p > 0 ? std::exponential_distribution<>(p)(rng_) : std::numeric_limits<double>::infinity());
    }
}
// R4 & R6
    void Vessel::simulate(double end_time, std::function<void(double, const std::vector<std::shared_ptr<Species>>&)> observer) {
    double time = 0.0;
    while (time <= end_time) {
        update_all_delays();
        auto next = find_next_reaction();

        // 🧪 DEBUG
        std::cout << "[SIM] next reaction ID: " << next->id()
                  << ", delay: " << next->delay() << std::endl;

        double dt = next->delay();
        if (dt == std::numeric_limits<double>::infinity()) {
            std::cout << "[SIM] All propensities are zero — exiting.\n";
            break;
        }

        time += dt;
        std::cout << "[DEBUG] Time: " << time << ", Executing reaction ID " << next->id() << std::endl;
        next->execute(species_);

        // 🧪 DEBUG
        std::cout << "[SIM] Executed reaction " << next->id()
                  << " at time " << time << std::endl;

        if (observer) observer(time, species_);
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
    std::stringstream ss;
    auto& r = reactions_[idx];
    for (const auto& [id, count] : r->reactants()) ss << species_[id]->name() << " + ";
    ss << "--(" << r->rate() << ")--> ";
    for (const auto& [id, count] : r->products()) ss << species_[id]->name() << " + ";
    return ss.str();
}
    // R2
std::string Vessel::to_string() const {
    std::stringstream ss;
    for (const auto& s : species_) ss << s->name() << "=" << s->count() << ", ";
    return ss.str();
}
    // R2
std::string Vessel::to_dot() const {
    std::stringstream ss;
    ss << "digraph {\n";
    for (const auto& s : species_) ss << "s" << s->id() << "[label=\"" << s->name() << "\",shape=box];\n";
    for (const auto& r : reactions_) {
        ss << "r" << r->id() << "[label=\"" << r->rate() << "\",shape=oval];\n";
        for (const auto& [id, count] : r->reactants()) ss << "s" << id << " -> r" << r->id() << ";\n";
        for (const auto& [id, count] : r->products()) ss << "r" << r->id() << " -> s" << id << ";\n";
    }
    ss << "}\n";
    return ss.str();
}

} // namespace Stochastic
