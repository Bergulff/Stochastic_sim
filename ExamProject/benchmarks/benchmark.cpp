//
// Created by Nicolai Bergulff on 14/06/2025.
//
#include "../include/Stochastic/stochastic.h"
#include <iostream>
#include <chrono>
#include <fstream>
#include <thread>
#include <iomanip>

//R10
using namespace Stochastic;
using Clock = std::chrono::high_resolution_clock;

void printBenchmarkHeader() {
    std::cout << std::string(90, '-') << "\n";
    std::cout << std::left << std::setw(40) << "Benchmark"
              << std::setw(20) << "Time"
              << std::setw(15) << "CPU"
              << std::setw(15) << "Iterations\n";
    std::cout << std::string(90, '-') << "\n";
}

void printBenchmarkResult(const std::string& name, double time_seconds, double cpu_time, int iterations) {
    std::cout << std::left << std::setw(40) << name;

    if (time_seconds >= 1.0) {
        std::cout << std::setw(20) << (std::to_string(time_seconds).substr(0, 4) + " s");
    } else {
        std::cout << std::setw(20) << (std::to_string(time_seconds * 1000).substr(0, 4) + " ms");
    }

    // Format CPU time
    if (cpu_time >= 1.0) {
        std::cout << std::setw(15) << (std::to_string(cpu_time).substr(0, 5) + " s");
    } else {
        std::cout << std::setw(15) << (std::to_string(cpu_time * 1000).substr(0, 5) + " ms");
    }

    std::cout << std::setw(15) << iterations << "\n";
}

void safeSimulate(Vessel& vessel, double end_time) {
    try {
        vessel.simulate(end_time);
    } catch (const SimulationException& e) {

    } catch (const std::exception& e) {
        std::cerr << "Simulation error: " << e.what() << std::endl;
        throw;
    }
}

int main() {
    const size_t N = 10000;
    const double end_time = 50.0;
    const size_t runs = 100;
    const size_t threads = std::thread::hardware_concurrency();

    std::cout << "Benchmarking SEIHR Covid-19 Model...\n";
    std::cout << "Population: " << N << ", End time: " << end_time << " days, Runs: " << runs << "\n\n";

    printBenchmarkHeader();

    auto t1 = Clock::now();
    for (size_t i = 0; i < runs; ++i) {
        Vessel copy = seir(N);
        safeSimulate(copy, end_time);
    }
    auto t2 = Clock::now();
    std::chrono::duration<double> serial_duration = t2 - t1;


    printBenchmarkResult("Simulation_singlecore", serial_duration.count(), serial_duration.count(), runs);

    // --- Multi Core Benchmarks ---
    std::vector<size_t> thread_counts = {2, 4, 8, 10, 16};
    Vessel base_model = seir(N);

    for (size_t num_threads : thread_counts) {
        if (num_threads > threads) continue;

        auto t3 = Clock::now();
        auto results = Simulator::runParallelSimulations<size_t>(
            base_model, end_time, runs,
            [](const Vessel& v) {

                Vessel copy = const_cast<Vessel&>(v);
                safeSimulate(copy, 50.0);

                auto* sp = copy.get_species("H").get();
                return sp->count();
            },
            num_threads
        );
        auto t4 = Clock::now();
        std::chrono::duration<double> parallel_duration = t4 - t3;


        double cpu_time_per_thread = parallel_duration.count() * num_threads;

        std::string benchmark_name = "gillespie_multicore/" + std::to_string(num_threads);
        printBenchmarkResult(benchmark_name, parallel_duration.count(), cpu_time_per_thread, runs);
    }

    // --- Export benchmark results ---
    std::ofstream out("benchmark_results.csv");
    if (!out) {
        std::cerr << "Failed to write benchmark_results.csv\n";
        return 1;
    }

    out << "Mode,Time,Speedup\n";
    out << "Single," << serial_duration.count() << ",1.0\n";

    for (size_t num_threads : thread_counts) {
        if (num_threads > threads) continue;

        auto t3 = Clock::now();
        auto results = Simulator::runParallelSimulations<size_t>(
            base_model, end_time, runs,
            [](const Vessel& v) {
                Vessel copy = const_cast<Vessel&>(v);
                safeSimulate(copy, 50.0);
                auto* sp = copy.get_species("H").get();
                return sp->count();
            },
            num_threads
        );
        auto t4 = Clock::now();
        std::chrono::duration<double> parallel_duration = t4 - t3;

        double speedup = serial_duration.count() / parallel_duration.count();
        out << "Parallel_" << num_threads << "," << parallel_duration.count() << "," << speedup << "\n";
    }

    std::cout << "\nBenchmark results written to benchmark_results.csv\n";
    std::cout << "Note: Simulations may terminate early when epidemic burns out - this is expected behavior.\n";
    return 0;
}