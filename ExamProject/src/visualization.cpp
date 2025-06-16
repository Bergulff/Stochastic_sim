//
// Created by Nicolai Bergulff on 12/06/2025.
//
#include "../include/Stochastic/stochastic.h"
#include <fstream>
#include <stdexcept>

namespace Stochastic {
    // R2
    std::string Visualization::generateDotGraph(const Vessel& vessel) {
        return vessel.to_dot();
    }
    // R2
    std::string Visualization::generateTextDescription(const Vessel& vessel) {
        return vessel.to_string();
    }
    // R6
    void Visualization::saveTrajectoryToCSV(const std::string& filename, const std::map<double, std::map<std::string, size_t>>& trajectory) {
        std::ofstream file(filename);
        if (!file.is_open()) throw std::runtime_error("Failed to open file " + filename);

        if (trajectory.empty()) return;
        const auto& headers = trajectory.begin()->second;

        file << "time";
        for (const auto& [name, _] : headers) file << "," << name;
        file << "\n";

        for (const auto& [time, values] : trajectory) {
            file << time;
            for (const auto& [_, count] : values) file << "," << count;
            file << "\n";
        }
    }
    // R6
    void Visualization::saveTrajectoryToGnuplot(const std::string& filename, const std::map<double, std::map<std::string, size_t>>& trajectory) {
        std::ofstream file(filename);
        if (!file.is_open()) throw std::runtime_error("Failed to open file " + filename);
        for (const auto& [time, values] : trajectory) {
            file << time;
            for (const auto& [_, count] : values) file << "\t" << count;
            file << "\n";
        }
    }

} // namespace Stochastic
