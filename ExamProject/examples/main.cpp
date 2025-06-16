#include "../include/Stochastic/stochastic.h"
#include <filesystem>
#include <iostream>
#include <numeric>
#include "../include/Stochastic/stochastic.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>

using namespace Stochastic;

int main() {
    const std::string out_dir = "output2";
    std::filesystem::create_directory(out_dir);

    // === Circadian Rhythm (R5, R6, R7) ===
    std::cout << "[Circadian] Running circadian rhythm simulation...\n";
    Vessel circadian = circadian_rhythm();

    TrajectoryObserver traj;
    Simulator::runSimulation(circadian, 200.0, &traj);

    const auto& trajectory = traj.getTrajectory();
    std::cout << "[DEBUG] Time points recorded: " << trajectory.size() << "\n";

    if (trajectory.empty()) {
        std::cerr << "[ERROR] No data recorded! Check if initial species like DA, DR, etc. have count > 0.\n";
    } else {
        // Print the first 10 time points
        size_t counter = 0;
        for (const auto& [t, snapshot] : trajectory) {
            if (counter++ >= 10) break;
            std::cout << t << ": ";
            for (const auto& [name, count] : snapshot)
                std::cout << name << "=" << count << " ";
            std::cout << "\n";
        }

        const std::string csv_path = out_dir + "/circadian.csv";
        try {
            //Visualization::saveTrajectoryToCSV(csv_path, trajectory);
            if (std::ofstream out("output2/circadian.csv"); !out.is_open()) {
                std::cerr << "[ERROR] Could not open output2/circadian.csv for writing.\n";
            } else {
                // Write header
                const auto& header = trajectory.begin()->second;
                out << "time";
                for (const auto& [species, _] : header)
                    out << "," << species;
                out << "\n";

                // Write rows
                for (const auto& [time, snapshot] : trajectory) {
                    out << time;
                    for (const auto& [species, _] : header) {
                        auto it = snapshot.find(species);
                        out << "," << (it != snapshot.end() ? std::to_string(it->second) : "0");
                    }
                    out << "\n";
                }

                std::cout << "[DEBUG] Manually wrote CSV to output2/circadian.csv\n";
            }
            std::cout << "[Circadian] Saved output to " << csv_path << "\n";
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Failed to save CSV: " << e.what() << "\n";
        }
    }

    // === SEIHR Example ===
    std::cout << "\n[Covid] Running SEIHR simulation...\n";
    Vessel covid = seir(10000);
    PeakObserver peak("H");

    Simulator::runSimulation(covid, 100.0, &peak);
    auto [time, value] = peak.getPeak();
    std::cout << "[Covid] Peak hospitalized: " << value << " at time " << time << "\n";

    auto peaks = Simulator::runParallelSimulations<size_t>(
        covid, 100.0, 100,
        [](const Vessel& v) {
            PeakObserver local("H");
            Simulator::runSimulation(const_cast<Vessel&>(v), 100.0, &local);
            return local.getPeak().second;
        });

    double avg_peak = std::accumulate(peaks.begin(), peaks.end(), 0.0) / peaks.size();
    std::cout << "[Covid] Average peak (100 sims): " << avg_peak << "\n";

    std::cout << "\nRun this to plot circadian:\n";
    std::cout << "gnuplot -e \"set datafile separator ','; set terminal png size 1000,600; "
                 "set output 'output2/circadian.png'; "
                 "plot 'output2/circadian.csv' using 1:2 with lines title columnhead(2), "
                 "'output2/circadian.csv' using 1:3 with lines title columnhead(3), "
                 "'output2/circadian.csv' using 1:4 with lines title columnhead(4)\"\n";

    return 0;
}
