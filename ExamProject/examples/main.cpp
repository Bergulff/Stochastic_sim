//
// Created by Nicolai Bergulff on 14/06/2025.
//
#include "../include/Stochastic/stochastic.h"
#include <filesystem>
#include <iostream>
#include <numeric>
#include <fstream>

using namespace Stochastic;

int main() {
    const std::string out_dir = "output";
    std::filesystem::create_directory(out_dir);

    // === R5, R6, R7 - Circadian Rhythm Simulation ===
    std::cout << "[Circadian] Running circadian rhythm simulation...\n";
    Vessel circadian = circadian_rhythm();

    // Print initial system state
    std::cout << "[DEBUG] Initial system state:\n" << circadian.to_string() << "\n";

    // R7 - Trajectory observer for recording simulation data
    TrajectoryObserver traj;
    Simulator::runSimulation(circadian, 200.0, &traj);

    const auto& trajectory = traj.getTrajectory();
    std::cout << "[DEBUG] Time points recorded: " << trajectory.size() << "\n";

    if (trajectory.empty()) {
        std::cerr << "[ERROR] No data recorded! Check simulation implementation.\n";
    } else {
        // Print sample data points
        size_t counter = 0;
        for (const auto& [t, snapshot] : trajectory) {
            if (counter++ >= 5) break;
            std::cout << "t=" << t << ": ";
            for (const auto& [name, count] : snapshot) {
                if (name != "environment") // Skip environment in output
                    std::cout << name << "=" << count << " ";
            }
            std::cout << "\n";
        }

        // R6 - Save trajectory to CSV for external visualization
        const std::string csv_path = out_dir + "/circadian.csv";
        try {
            std::ofstream out(csv_path);
            if (!out.is_open()) {
                std::cerr << "[ERROR] Could not open " << csv_path << " for writing.\n";
            } else {
                // Write header
                const auto& header = trajectory.begin()->second;
                out << "time";
                for (const auto& [species, _] : header) {
                    if (species != "environment") // Skip environment
                        out << "," << species;
                }
                out << "\n";

                // Write data rows
                for (const auto& [time, snapshot] : trajectory) {
                    out << time;
                    for (const auto& [species, _] : header) {
                        if (species != "environment") {
                            auto it = snapshot.find(species);
                            out << "," << (it != snapshot.end() ? std::to_string(it->second) : "0");
                        }
                    }
                    out << "\n";
                }
                std::cout << "[Circadian] Saved trajectory to " << csv_path << "\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Failed to save CSV: " << e.what() << "\n";
        }
    }

    // R2 - Generate DOT graph for visualization
    try {
        std::string dot_content = circadian.to_dot();
        std::ofstream dot_file(out_dir + "/circadian.dot");
        if (dot_file.is_open()) {
            dot_file << dot_content;
            std::cout << "[Circadian] Saved DOT graph to " << out_dir << "/circadian.dot\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to save DOT file: " << e.what() << "\n";
    }

    // === R5, R7, R8 - SEIHR COVID-19 Model ===
    std::cout << "\n[Covid] Running SEIHR simulation...\n";

    // Test with different population sizes as specified in assignment
    const std::vector<uint32_t> populations = {10000, 589755, 5822763}; // Test, NNJ, NDK

    for (uint32_t N : populations) {
        std::cout << "[Covid] Testing population N = " << N << "\n";

        Vessel covid = seir(N);

        // R7 - Peak observer to find maximum hospitalization
        PeakObserver peak("H");
        Simulator::runSimulation(covid, 100.0, &peak);
        auto [time, value] = peak.getPeak();
        std::cout << "[Covid] Population " << N << " - Peak hospitalized: " << value
                  << " at time " << time << " days\n";

        // R8 - Parallel simulation for statistical analysis
        if (N == 10000) { // Only run extensive parallel simulation for smaller population
            std::cout << "[Covid] Running 100 parallel simulations...\n";
            auto peaks = Simulator::runParallelSimulations<size_t>(
                covid, 100.0, 100,
                [](const Vessel& v) {
                    PeakObserver local("H");
                    Vessel copy = const_cast<Vessel&>(v); // Create mutable copy
                    Simulator::runSimulation(copy, 100.0, &local);
                    return local.getPeak().second;
                });

            double avg_peak = std::accumulate(peaks.begin(), peaks.end(), 0.0) / peaks.size();
            double max_peak = *std::max_element(peaks.begin(), peaks.end());
            double min_peak = *std::min_element(peaks.begin(), peaks.end());

            std::cout << "[Covid] Population " << N << " - Statistics over 100 simulations:\n";
            std::cout << "  Average peak: " << avg_peak << "\n";
            std::cout << "  Maximum peak: " << max_peak << "\n";
            std::cout << "  Minimum peak: " << min_peak << "\n";

            // Save statistics to file
            std::ofstream stats_file(out_dir + "/covid_stats.txt");
            if (stats_file.is_open()) {
                stats_file << "COVID-19 SEIHR Model Statistics (N=" << N << ")\n";
                stats_file << "Simulations: 100\n";
                stats_file << "Simulation time: 100 days\n";
                stats_file << "Average peak hospitalization: " << avg_peak << "\n";
                stats_file << "Maximum peak hospitalization: " << max_peak << "\n";
                stats_file << "Minimum peak hospitalization: " << min_peak << "\n";
            }
        }
    }

    // === R2 - Pretty printing demonstrations ===
    std::cout << "\n=== R2 - System Description ===\n";
    std::cout << "Circadian Rhythm System:\n" << circadian.to_string() << "\n";

    // Provide visualization commands
    std::cout << "\n=== Visualization Commands ===\n";
    std::cout << "To generate circadian rhythm plot with gnuplot:\n";
    std::cout << "gnuplot -e \"set datafile separator ','; set terminal png size 1200,800; "
                 "set output '" << out_dir << "/circadian.png'; "
                 "set xlabel 'Time (hours)'; set ylabel 'Molecule Count'; "
                 "set title 'Circadian Rhythm Simulation'; "
                 "plot '" << out_dir << "/circadian.csv' using 1:2 with lines title columnhead(2), "
                 "'' using 1:3 with lines title columnhead(3), "
                 "'' using 1:4 with lines title columnhead(4), "
                 "'' using 1:5 with lines title columnhead(5)\"\n\n";

    std::cout << "To generate reaction network graph:\n";
    std::cout << "dot -Tpng " << out_dir << "/circadian.dot -o " << out_dir << "/circadian_network.png\n\n";

    std::cout << "Simulation completed successfully!\n";
    std::cout << "Check the '" << out_dir << "' directory for output files.\n";

    return 0;
}
