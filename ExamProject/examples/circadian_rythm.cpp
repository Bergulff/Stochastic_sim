//
// Created by Nicolai Bergulff on 14/06/2025.
//
#include "../include/Stochastic/stochastic.h"
#include <iostream>

namespace Stochastic {

    // R5 - Circadian rhythm model implementation
    Vessel circadian_rhythm() {
        // Rate constants from the assignment
        const auto alphaA = 50.0;      // MA production from DA
        const auto alphaMA = 500.0;    // MA production from D'A
        const auto alphaR = 0.01;      // MR production from DR
        const auto alphaR_R = 50.0;    // MR production from D'R
        const auto betaA = 50.0;       // A production from MA
        const auto betaR = 5.0;        // R production from MR
        const auto gammaA = 1.0;       // A + DA -> D'A
        const auto gammaR = 1.0;       // A + DR -> D'R
        const auto gammaC = 2.0;       // A + R -> C
        const auto deltaA = 1.0;       // C -> R (and A decay)
        const auto deltaR = 0.2;       // R decay
        const auto deltaMA = 10.0;     // MA decay
        const auto deltaMR = 0.5;      // MR decay
        const auto thetaA = 50.0;      // D'A -> DA + A
        const auto thetaR = 100.0;     // D'R -> DR + A

        auto v = Vessel("Circadian Rhythm");

        // Create species with appropriate initial conditions
        const auto env = v.environment();
        const auto DA = v.add("DA", 1);      // DNA binding site for A
        const auto D_A = v.add("D_A", 0);    // DNA bound by A
        const auto DR = v.add("DR", 1);      // DNA binding site for R
        const auto D_R = v.add("D_R", 0);    // DNA bound by A
        const auto MA = v.add("MA", 0);      // mRNA for A
        const auto MR = v.add("MR", 0);      // mRNA for R
        const auto A = v.add("A", 0);        // Protein A
        const auto R = v.add("R", 0);        // Protein R
        const auto C = v.add("C", 0);        // Complex A-R

        // Binding reactions: A binds to DNA sites
        v.add((*A + *DA) >> gammaA >>= *D_A);         // A + DA -> D'A
        v.add(*D_A >> thetaA >>= *DA + *A);           // D'A -> DA + A
        v.add((*A + *DR) >> gammaR >>= *D_R);         // A + DR -> D'R
        v.add(*D_R >> thetaR >>= *DR + *A);           // D'R -> DR + A

        // Transcription reactions: DNA sites produce mRNA
        v.add(*D_A >> alphaMA >>= *MA + *D_A);        // D'A -> MA + D'A (catalytic)
        v.add(*DA >> alphaA >>= *MA + *DA);           // DA -> MA + DA (catalytic)
        v.add(*D_R >> alphaR_R >>= *MR + *D_R);       // D'R -> MR + D'R (catalytic)
        v.add(*DR >> alphaR >>= *MR + *DR);           // DR -> MR + DR (catalytic)

        // Translation reactions: mRNA produces proteins
        v.add(*MA >> betaA >>= *MA + *A);             // MA -> MA + A (catalytic)
        v.add(*MR >> betaR >>= *MR + *R);             // MR -> MR + R (catalytic)

        // Complex formation and degradation
        v.add((*A + *R) >> gammaC >>= *C);            // A + R -> C
        v.add(*C >> deltaA >>= *R);                   // C -> R (A is degraded)

        // Degradation reactions to environment
        v.add(*A >> deltaA >>= *env);                 // A -> environment
        v.add(*R >> deltaR >>= *env);                 // R -> environment
        v.add(*MA >> deltaMA >>= *env);               // MA -> environment
        v.add(*MR >> deltaMR >>= *env);               // MR -> environment

        return v;
    }

}
namespace Stochastic {
    int main() {
        auto v = circadian_rhythm();

        TrajectoryObserver obs;
        Simulator::runSimulation(v, 48.0, &obs);
        obs.saveToFile("circadian_trajectory.csv");

        std::cout << "Circadian simulation complete.\n";
        return 0;
    }
} // namespace Stochastic