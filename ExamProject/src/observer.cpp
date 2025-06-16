//
// Created by Nicolai Bergulff on 14/06/2025.
//
#include "../include/Stochastic/stochastic.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace Stochastic {
    // R6
    void TrajectoryObserver::observe(double time, const std::vector<Species*>& species) {
        std::cout << "[OBSERVER] Recording at time: " << time << std::endl;

        std::map<std::string, size_t> snapshot;
        for (const auto* s : species) snapshot[s->name()] = s->count();
        trajectory_[time] = std::move(snapshot);
    }

    const std::map<double, std::map<std::string, size_t>>& TrajectoryObserver::getTrajectory() const {
        return trajectory_;
    }
    // R6
    void TrajectoryObserver::saveToFile(const std::string& filename) const {
        std::ofstream out(filename);
        if (!out) throw std::runtime_error("Cannot open file: " + filename);

        if (trajectory_.empty()) return;

        const auto& first_row = trajectory_.begin()->second;
        out << "time";
        for (const auto& [name, _] : first_row) out << "," << name;
        out << "\n";

        for (const auto& [time, snapshot] : trajectory_) {
            out << time;
            for (const auto& [_, count] : snapshot) out << "," << count;
            out << "\n";
        }
    }
    // R7
    PeakObserver::PeakObserver(const std::string& species_name)
        : species_name_(species_name), peak_time_(0.0), peak_value_(0) {}

    void PeakObserver::observe(double time, const std::vector<Species*>& species) {
        for (const auto* s : species) {
            if (s->name() == species_name_ && s->count() > peak_value_) {
                peak_value_ = s->count();
                peak_time_ = time;
            }
        }
    }

    std::pair<double, size_t> PeakObserver::getPeak() const {
        return {peak_time_, peak_value_};
    }

    // Note: FunctionObserver<T> is implemented inline in the header to support templating.

} // namespace Stochastic
