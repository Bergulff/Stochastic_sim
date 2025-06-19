//
// Created by Nicolai Bergulff on 14/06/2025.
//
#include "../include/Stochastic/stochastic.h"
#include <iostream>
#include <fstream>

using namespace Stochastic;


int main() {
    // R5
    Vessel model = circadian_rhythm();

    // R7
    TrajectoryObserver observer;
    Simulator::runSimulation(model, 200.0, &observer);

    // R6
    observer.saveToFile("output/circadian.csv");

    // R2
    std::string dot = model.to_dot();
    std::ofstream dot_file("output/circadian.dot");
    dot_file << dot;

    return 0;
}
