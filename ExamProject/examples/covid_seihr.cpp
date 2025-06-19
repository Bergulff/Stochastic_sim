//
// Created by Nicolai Bergulff on 14/06/2025.
//
#include "../include/Stochastic/stochastic.h"
#include <iostream>
#include <cmath>

namespace Stochastic {

     Vessel seir(uint32_t N) {
        auto v = Vessel("COVID19 SEIHR: " + std::to_string(N));


        const auto eps = 0.0009;
        const auto I0 = size_t(std::round(eps * N));
        const auto E0 = size_t(std::round(eps * N * 15));
        const auto S0 = N - I0 - E0;

        const auto R0 = 2.4;
        const auto alpha = 1.0 / 5.1;
        const auto gamma = 1.0 / 3.1;
        const auto beta = R0 * gamma;

        const auto P_H = 9e-3;
        const auto kappa = gamma * P_H * (1.0 - P_H);
        const auto tau = 1.0 / 10.12;


        const auto S = v.add("S", S0);
        const auto E = v.add("E", E0);
        const auto I = v.add("I", I0);
        const auto H = v.add("H", 0);
        const auto R = v.add("R", 0);


        v.add((*S + *I) >> (beta / N) >>= (*E + *I));
        v.add(*E >> alpha >>= *I);
        v.add(*I >> gamma >>= *R);
        v.add(*I >> kappa >>= *H);
        v.add(*H >> tau >>= *R);

        return v;
    }

}