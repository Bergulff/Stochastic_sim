//
// Created by Nicolai Bergulff on 14/06/2025.
//
#include "../include/Stochastic/stochastic.h"
#include <iostream>

namespace Stochastic {

    // R5
    Vessel circadian_rhythm() {

        const auto alphaA = 50.0;
        const auto alphaMA = 500.0;
        const auto alphaR = 0.01;
        const auto alphaR_R = 50.0;
        const auto betaA = 50.0;
        const auto betaR = 5.0;
        const auto gammaA = 1.0;
        const auto gammaR = 1.0;
        const auto gammaC = 2.0;
        const auto deltaA = 1.0;
        const auto deltaR = 0.2;
        const auto deltaMA = 10.0;
        const auto deltaMR = 0.5;
        const auto thetaA = 50.0;
        const auto thetaR = 100.0;

        auto v = Vessel("Circadian Rhythm");


        const auto env = v.environment();
        const auto DA = v.add("DA", 1);
        const auto D_A = v.add("D_A", 0);
        const auto DR = v.add("DR", 1);
        const auto D_R = v.add("D_R", 0);
        const auto MA = v.add("MA", 0);
        const auto MR = v.add("MR", 0);
        const auto A = v.add("A", 0);
        const auto R = v.add("R", 0);
        const auto C = v.add("C", 0);


        v.add((*A + *DA) >> gammaA >>= *D_A);
        v.add(*D_A >> thetaA >>= *DA + *A);
        v.add((*A + *DR) >> gammaR >>= *D_R);
        v.add(*D_R >> thetaR >>= *DR + *A);


        v.add(*D_A >> alphaMA >>= *MA + *D_A);
        v.add(*DA >> alphaA >>= *MA + *DA);
        v.add(*D_R >> alphaR_R >>= *MR + *D_R);
        v.add(*DR >> alphaR >>= *MR + *DR);


        v.add(*MA >> betaA >>= *MA + *A);
        v.add(*MR >> betaR >>= *MR + *R);


        v.add((*A + *R) >> gammaC >>= *C);
        v.add(*C >> deltaA >>= *R);


        v.add(*A >> deltaA >>= *env);
        v.add(*R >> deltaR >>= *env);
        v.add(*MA >> deltaMA >>= *env);
        v.add(*MR >> deltaMR >>= *env);

        return v;
    }

} // namespace Stochastic