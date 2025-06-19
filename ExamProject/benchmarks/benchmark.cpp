#include "../include/Stochastic/stochastic.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <fstream>

using namespace Stochastic;
using Clock = std::chrono::high_resolution_clock;

void printBenchmarkHeader(std::ostream& out) {
    out << "Benchmark Time CPU Iterations\n";
}

void printBenchmarkResult(std::ostream& out, const std::string& name, double time_seconds, double cpu_time, int iterations) {
    out << name << " "
        << std::fixed << std::setprecision(2) << (time_seconds * 1000) << " ms "
        << std::fixed << std::setprecision(3) << (cpu_time * 1000) << " ms "
        << iterations << "\n";
}

int main() {
    const size_t N = 10000;
    const double end_time = 50.0;
    const size_t runs = 100;
    const size_t threads = std::thread::hardware_concurrency();

    std::vector<size_t> thread_counts = {2, 4, 8, 10, 16};
    Vessel base_model = seir(N);

    std::ofstream outfile("benchmark_results.txt");
    if (!outfile) {
        std::cerr << "Failed to open output file.\n";
        return 1;
    }

    printBenchmarkHeader(outfile);

    // Serial benchmark
    auto t1 = Clock::now();
    for (size_t i = 0; i < runs; ++i) {
        Vessel copy = seir(N);
        PeakObserver peak("H");
        Simulator::runSimulation(copy, end_time, &peak);
    }
    auto t2 = Clock::now();
    std::chrono::duration<double> serial_duration = t2 - t1;
    printBenchmarkResult(outfile, "SIMULATE_BM/Simulations:100", serial_duration.count(), serial_duration.count(), runs);

    // Parallel benchmarks
    for (size_t num_threads : thread_counts) {
        if (num_threads > threads) continue;

        auto t3 = Clock::now();
        auto results = Simulator::runParallelSimulations<size_t>(
            base_model, end_time, runs,
            [](const Vessel& v) {
                Vessel copy(v);
                PeakObserver peak("H");
                Simulator::runSimulation(copy, 50.0, &peak);
                return peak.getPeak().second;
            },
            num_threads
        );
        auto t4 = Clock::now();
        std::chrono::duration<double> parallel_duration = t4 - t3;
        double cpu_time = parallel_duration.count() * num_threads;

        std::string label = "SIMULATE_PARALLEL_BM/Simulations:100/Threads:" + std::to_string(num_threads);
        printBenchmarkResult(outfile, label, parallel_duration.count(), cpu_time, 1000);
    }

    outfile.close();
    return 0;
}
