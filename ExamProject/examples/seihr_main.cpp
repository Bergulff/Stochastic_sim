//
// Created by Nicolai Bergulff on 14/06/2025.
//
// examples/seir_main.cpp
#include "../include/Stochastic/stochastic.h"
#include <iostream>

using namespace Stochastic;

int main() {
    auto v = seir(10000);  // Uses function defined in covid_seihr.cpp

    TrajectoryObserver obs;
    Simulator::runSimulation(v, 100.0, &obs);

    obs.saveToFile("seihr_trajectory.csv");

    std::cout << "SEIHR simulation complete.\n";
    return 0;
}
