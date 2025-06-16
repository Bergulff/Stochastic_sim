//
// Created by Nicolai Bergulff on 14/06/2025.
//
#include "../include/Stochastic/stochastic.h"
#include <iostream>
#include <chrono>
#include <fstream>
#include <thread>

//R10
using namespace Stochastic;
using Clock = std::chrono::high_resolution_clock;

int main() {
    const size_t N = 10000;
    const double end_time = 100.0;
    const size_t runs = 100;
    const size_t threads = std::thread::hardware_concurrency();

    std::cout << "Benchmarking SEIHR Covid-19 Model...\n";

    // Build model once for parallel simulation
    Vessel base_model = seir(N);

    // --- Single Core Benchmark ---
    auto t1 = Clock::now();
    for (size_t i = 0; i < runs; ++i) {
        Vessel copy = seir(N);
        copy.simulate(end_time);
    }
    auto t2 = Clock::now();
    std::chrono::duration<double> serial_duration = t2 - t1;

    std::cout << "Single-threaded (" << runs << " runs): "
              << serial_duration.count() << " seconds\n";

    // --- Multi Core Benchmark ---
    auto t3 = Clock::now();
    auto results = Simulator::runParallelSimulations<size_t>(
        base_model, end_time, runs,
        [](const Vessel& v) {
            auto* sp = v.get_species("H").get();
            return sp->count();
        },
        threads
    );
    auto t4 = Clock::now();
    std::chrono::duration<double> parallel_duration = t4 - t3;

    std::cout << "Multithreaded (" << threads << " threads, " << runs << " runs): "
              << parallel_duration.count() << " seconds\n";

    // --- Export benchmark results ---
    std::ofstream out("benchmark_results.csv");
    if (!out) {
        std::cerr << "Failed to write benchmark_results.csv\n";
        return 1;
    }

    out << "Mode,Time\n";
    out << "Single," << serial_duration.count() << "\n";
    out << "Parallel," << parallel_duration.count() << "\n";

    std::cout << "Benchmark results written to benchmark_results.csv\n";
    return 0;
}
