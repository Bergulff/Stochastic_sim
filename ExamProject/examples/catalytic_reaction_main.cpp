//
// Created by Nicolai Bergulff on 16/06/2025.
//
#include "../include/Stochastic/stochastic.h"
#include <iostream>

using namespace Stochastic;

// Forward declaration
void test_rate_dependency();

int main() {
    std::cout << "Catalytic Reaction Examples\n";

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
        std::cout << "OT Graph Output\n ";
        Vessel demo = simple_catalytic(100, 0, 1);
        std::cout << demo.to_dot() << "\n";
    }

    // R8
    {
        std::cout << "Parallel Simulation Example\n";
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

        std::cout << "Average B count after 2000 time units: " << avg << "\n";
    }

    // R9
    std::cout << "Rate Dependency Test";
    test_rate_dependency();

    return 0;
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