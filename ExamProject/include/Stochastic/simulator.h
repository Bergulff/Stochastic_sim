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
