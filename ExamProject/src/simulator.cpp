//
// Created by Nicolai Bergulff on 12/06/2025.
//
#include "../include/Stochastic/stochastic.h"
#include <future>
#include <thread>

namespace Stochastic {

    // R4
    void Simulator::runSimulation(Vessel& vessel, double endTime, Observer* observer) {
        std::function<void(double, const std::vector<std::shared_ptr<Species>>&)> cb = nullptr;

        if (observer) {
            cb = [&](double t, const std::vector<std::shared_ptr<Species>>& sps) {
                std::vector<Species*> raw;
                for (const auto& sp : sps) raw.push_back(sp.get());
                observer->observe(t, raw);
            };
        }

        vessel.simulate(endTime, cb);
    }

    // R8
    template<typename ResultType>
    std::vector<ResultType> Simulator::runParallelSimulations(
        Vessel& vessel,
        double endTime,
        size_t numSimulations,
        std::function<ResultType(const Vessel&)> resultExtractor,
        size_t numThreads) {

        std::vector<ResultType> results(numSimulations);
        std::vector<std::future<void>> futures;

        // Calculate work distribution
        size_t simsPerThread = numSimulations / numThreads;
        size_t leftover = numSimulations % numThreads;

        size_t index = 0;
        for (size_t t = 0; t < numThreads; ++t) {
            size_t count = simsPerThread + (t < leftover ? 1 : 0);
            if (count == 0) continue;

            size_t start = index, end = start + count;

            // R8
            futures.push_back(std::async(std::launch::async, [&, start, end]() {
                for (size_t i = start; i < end; ++i) {
                    Vessel copy = vessel;
                    copy.simulate(endTime);
                    results[i] = resultExtractor(copy);
                }
            }));
            index = end;
        }

        for (auto& f : futures) f.get();
        return results;
    }

    // R8
    template std::vector<size_t> Simulator::runParallelSimulations<size_t>(
        Vessel&, double, size_t, std::function<size_t(const Vessel&)>, size_t);

    template std::vector<double> Simulator::runParallelSimulations<double>(
        Vessel&, double, size_t, std::function<double(const Vessel&)>, size_t);
} // namespace Stochastic