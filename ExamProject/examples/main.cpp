//
// Created by Nicolai Bergulff on 14/06/2025.
//
#include "../include/Stochastic/stochastic.h"
#include <filesystem>
#include <iostream>
#include <numeric>
#include <fstream>
#include <atomic>
#include <algorithm>

using namespace Stochastic;

int main() {
    // R5
    const std::string out_dir = "output";
    std::filesystem::create_directory(out_dir);

    // R5
    Vessel circadian = circadian_rhythm();

    // R7
    TrajectoryObserver circadian_traj;
    Simulator::runSimulation(circadian, 200.0, &circadian_traj);

    // R6
    circadian_traj.saveToFile(out_dir + "/circadian.csv");

    // R2
    std::string dot_content = circadian.to_dot();
    std::ofstream dot_file(out_dir + "/circadian.dot");
    dot_file << dot_content;

    // R5
    const std::vector<uint32_t> populations = {10000, 589755, 5822763};
    for (uint32_t N : populations) {
        Vessel covid = seir(N);

        // R7
        TrajectoryObserver covid_traj;
        Simulator::runSimulation(covid, 100.0, &covid_traj);

        // R6
        std::string filename = out_dir + "/covid_N" + std::to_string(N) + ".csv";
        covid_traj.saveToFile(filename);

        // R8
        if (N == 589755) {
            auto peaks = Simulator::runParallelSimulations<size_t>(
                covid, 100.0, 20,
                [](const Vessel& v) -> size_t {
                    Vessel vessel_copy(v);
                    PeakObserver local("H");
                    Simulator::runSimulation(vessel_copy, 100.0, &local);
                    auto peak_result = local.getPeak().second;
                    return peak_result;
                });

            double avg_peak = std::accumulate(peaks.begin(), peaks.end(), 0.0) / peaks.size();
            auto max_peak = *std::max_element(peaks.begin(), peaks.end());
            auto min_peak = *std::min_element(peaks.begin(), peaks.end());
            size_t zero_count = std::count(peaks.begin(), peaks.end(), 0);
        }
    }

    // R5
    {
        Vessel catalytic1 = simple_catalytic(100, 0, 1);
        TrajectoryObserver traj_cat1;
        Simulator::runSimulation(catalytic1, 2000.0, &traj_cat1);
        traj_cat1.saveToFile(out_dir + "/catalytic_scenario1.csv");
    }

    {
        Vessel catalytic2 = simple_catalytic(100, 0, 2);
        TrajectoryObserver traj_cat2;
        Simulator::runSimulation(catalytic2, 1500.0, &traj_cat2);
        traj_cat2.saveToFile(out_dir + "/catalytic_scenario2.csv");
    }

    {
        Vessel catalytic3 = simple_catalytic(50, 50, 1);
        TrajectoryObserver traj_cat3;
        Simulator::runSimulation(catalytic3, 1500.0, &traj_cat3);
        traj_cat3.saveToFile(out_dir + "/catalytic_scenario3.csv");
    }

    // R2
    Vessel catalytic_demo = simple_catalytic(100, 0, 1);
    catalytic_demo.to_string();

    return 0;
}