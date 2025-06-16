//
// Created by Nicolai Bergulff on 14/06/2025.
//
#include "../include/Stochastic/stochastic.h"
#include <iostream>

using namespace Stochastic;

int main() {
    auto v = circadian_rhythm();

    TrajectoryObserver obs;
    Simulator::runSimulation(v, 48.0, &obs);
    obs.saveToFile("circadian_trajectory.csv");

    std::cout << "Circadian simulation complete.\n";
    return 0;
}
