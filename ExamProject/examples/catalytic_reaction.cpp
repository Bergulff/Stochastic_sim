//
// Created by Nicolai Bergulff on 16/06/2025.
//
// Simple catalytic reaction example: A + C -> B + C
// This implements the reaction from Figure 1 in the assignment PDF
// Requirement 5 - Third example system

#include "Stochastic/stochastic.h"
#include <iostream>

namespace Stochastic {

Vessel simple_catalytic(size_t A0, size_t B0, size_t C_count, double lambda) {
    auto v = Vessel{"Simple Catalytic: A + C -> B + C"};

    // R5
    auto A = v.add("A", A0);
    auto B = v.add("B", B0);
    auto C = v.add("C", C_count);

    v.add((*A + *C) >> lambda >>= (*B + *C));

    return v;
}

// R9
void test_rate_dependency() {
    std::cout << "Testing Rate Dependency\n";

    std::vector<double> rates = {0.0001, 0.001, 0.01, 0.1};
    for (double rate : rates) {
        Vessel v = simple_catalytic(100, 0, 1, rate);

        TrajectoryObserver traj;
        Simulator::runSimulation(v, 1000.0, &traj);

        double half_life = -1;
        for (const auto& [time, counts] : traj.getTrajectory()) {
            if (counts.at("A") <= 50) {
                half_life = time;
                break;
            }
        }

        std::cout << "Rate: " << rate << ", Half-life: " << half_life << " time units\n";
    }
}

} // namespace Stochastic

