//
// Created by Nicolai Bergulff on 12/06/2025.
//
#ifndef STOCHASTIC_OBSERVER_H
#define STOCHASTIC_OBSERVER_H
#include <map>
#include <vector>
#include <string>
#include <functional>

namespace Stochastic {
    class Species;
    // R7
    class Observer {
    public:
        virtual ~Observer() = default;
        virtual void observe(double time, const std::vector<Species*>& species) = 0;
    };

    class TrajectoryObserver : public Observer {
        std::map<double, std::map<std::string, size_t>> trajectory_;
    public:
        void observe(double time, const std::vector<Species*>& species) override;
        const std::map<double, std::map<std::string, size_t>>& getTrajectory() const;
        void saveToFile(const std::string& filename) const;
    };

    template<typename T>
    class FunctionObserver : public Observer {
        std::function<T(double, const std::vector<Species*>&)> function_;
        std::vector<T> values_;
    public:
        // R7
        explicit FunctionObserver(std::function<T(double, const std::vector<Species*>&)> func);
        void observe(double time, const std::vector<Species*>& species) override;
        const std::vector<T>& getValues() const;
    };

    class PeakObserver : public Observer {
        std::string species_name_;
        double peak_time_ = 0.0;
        size_t peak_value_ = 0;
    public:
        explicit PeakObserver(const std::string& speciesName);
        void observe(double time, const std::vector<Species*>& species) override;
        std::pair<double, size_t> getPeak() const;
    };
}
#endif // STOCHASTIC_OBSERVER_H
