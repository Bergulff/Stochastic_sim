//
// Created by Nicolai Bergulff on 12/06/2025.
//
#ifndef STOCHASTIC_SIMULATOR_H
#define STOCHASTIC_SIMULATOR_H
#include <thread>
#include <future>
#include <vector>
#include <functional>
#include "vessel.h"
#include "observer.h"

namespace Stochastic {
    class Simulator {
    public:
        // R4
        static void runSimulation(Vessel& vessel, double endTime, Observer* observer = nullptr);

        //R8
        template<typename ResultType>
        static std::vector<ResultType> runParallelSimulations(
            Vessel& vessel,
            double endTime,
            size_t numSimulations,
            std::function<ResultType(const Vessel&)> resultExtractor,
            size_t numThreads = std::thread::hardware_concurrency()
        );
    };
}
#endif // STOCHASTIC_SIMULATOR_H


/*#ifndef SIMULATOR_H
#define SIMULATOR_H
#pragma once
#include <vector>
#include <thread>
#include <future>
#include "vessel.h"
#include "observer.h"

namespace Stochastic {

    class Simulator {
    public:
        // Run a single simulation
        static void runSimulation(Vessel& vessel, double endTime, Observer* observer = nullptr);

        // Run multiple simulations in parallel
        template<typename ResultType>
        static std::vector<ResultType> runParallelSimulations(
            Vessel& vessel,
            double endTime,
            size_t numSimulations,
            std::function<ResultType(const Vessel&)> resultExtractor,
            size_t numThreads = std::thread::hardware_concurrency()
        ) {
            std::vector<ResultType> results(numSimulations);
            std::vector<std::future<void>> futures;

            // Divide simulations among threads
            size_t simulationsPerThread = numSimulations / numThreads;
            size_t remainingSimulations = numSimulations % numThreads;

            size_t startIndex = 0;

            for (size_t i = 0; i < numThreads; ++i) {
                size_t count = simulationsPerThread + (i < remainingSimulations ? 1 : 0);
                size_t endIndex = startIndex + count;

                if (count == 0) continue;

                futures.push_back(std::async(std::launch::async, [&, startIndex, endIndex]() {
                    for (size_t j = startIndex; j < endIndex; ++j) {
                        // Create a copy of the vessel for this simulation
                        Vessel vesselCopy = vessel;
                        runSimulation(vesselCopy, endTime);
                        results[j] = resultExtractor(vesselCopy);
                    }
                }));

                startIndex = endIndex;
            }

            // Wait for all threads to complete
            for (auto& future : futures) {
                future.wait();
            }

            return results;
        }
    };

} // namespace stochastic
#endif //SIMULATOR_H*/
