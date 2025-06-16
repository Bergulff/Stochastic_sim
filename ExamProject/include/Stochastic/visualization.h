//
// Created by Nicolai Bergulff on 12/06/2025.
//
#ifndef STOCHASTIC_VISUALIZATION_H
#define STOCHASTIC_VISUALIZATION_H
#include <string>
#include <map>
#include <vector>

namespace Stochastic {
    class Vessel;
    class Reaction;

    class Visualization {
    public:
        //R2
        static std::string generateDotGraph(const Vessel& vessel);
        //R2
        static std::string generateTextDescription(const Vessel& vessel);

        //R6
        static void saveTrajectoryToCSV(const std::string& filename, const std::map<double, std::map<std::string, size_t>>& trajectory);
        static void saveTrajectoryToGnuplot(const std::string& filename, const std::map<double, std::map<std::string, size_t>>& trajectory);
    };
}
#endif // STOCHASTIC_VISUALIZATION_H

