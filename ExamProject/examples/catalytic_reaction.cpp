//
// Created by Nicolai Bergulff on 16/06/2025.
//
// Simple catalytic reaction example: A + C -> B + C
// This implements the reaction from Figure 1 in the assignment PDF
// Requirement 5 - Third example system

#include "Stochastic/stochastic.h"
#include <iostream>

namespace Stochastic {

// R5
Vessel simple_catalytic(size_t A0, size_t B0, size_t C_count, double lambda = 0.001) {
    auto v = Vessel{"Simple Catalytic: A + C -> B + C"};

    // R5
    auto A = v.add("A", A0);
    auto B = v.add("B", B0);
    auto C = v.add("C", C_count);

    v.add((*A + *C) >> lambda >>= (*B + *C));

    return v;
}

} // namespace Stochastic
int main() {
    using namespace Stochastic;

    std::cout << "=== Requirement 5: Simple Catalytic Reaction Examples ===\n\n";

    // R5
    {
        std::cout << "Scenario 1: A(0)=100, B(0)=0, C=1\n";
        Vessel v1 = simple_catalytic(100, 0, 1);

        std::cout << v1.to_string() << "\n";

        TrajectoryObserver traj1;
        Simulator::runSimulation(v1, 2000.0, &traj1);
        traj1.saveToFile("catalytic_scenario1.csv");
        std::cout << "Trajectory saved to catalytic_scenario1.csv\n\n";
    }

    // R5
    {
        std::cout << "Scenario 2: A(0)=100, B(0)=0, C=2\n";
        Vessel v2 = simple_catalytic(100, 0, 2);

        TrajectoryObserver traj2;
        Simulator::runSimulation(v2, 1500.0, &traj2);
        traj2.saveToFile("catalytic_scenario2.csv");
        std::cout << "Trajectory saved to catalytic_scenario2.csv\n\n";
    }

    // R5
    {
        std::cout << "Scenario 3: A(0)=50, B(0)=50, C=1\n";
        Vessel v3 = simple_catalytic(50, 50, 1);

        TrajectoryObserver traj3;
        Simulator::runSimulation(v3, 1500.0, &traj3);
        traj3.saveToFile("catalytic_scenario3.csv");
        std::cout << "Trajectory saved to catalytic_scenario3.csv\n\n";
    }

    // R2
    {
        std::cout << "=== DOT Graph Output ===\n";
        Vessel demo = simple_catalytic(100, 0, 1);
        std::cout << demo.to_dot() << "\n";
    }

    // R8
    {
        std::cout << "Parallel Simulation Example ";
        Vessel base = simple_catalytic(100, 0, 1);


        auto results = Simulator::runParallelSimulations<size_t>(
            base, 2000.0, 50,
            [](const Vessel& v) {
                return v.get_species("B")->count();
            }
        );

        double avg = 0.0;
        for (size_t count : results) avg += count;
        avg /= results.size();

        std::cout << "Average final B count over 50 simulations: " << avg << "\n";
        std::cout << "Expected: ~100 (all A should convert to B)\n";
    }

    return 0;
}

namespace Stochastic {

// R9
void test_rate_dependency() {
    std::cout << "Testing Rate Dependency";

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