//
// Created by Nicolai Bergulff on 14/06/2025.
//

#ifndef STOCHASTIC_H
#define STOCHASTIC_H

#pragma once

// Main header that includes all components of the stochastic simulation library
#include "species.h"
#include "reaction.h"
#include "vessel.h"
#include "simulator.h"
#include "observer.h"
#include "visualization.h"
#include "symbol_table.h"
#include "exceptions.h"

namespace Stochastic {
    Vessel seir(uint32_t N);  // Covid-19 SEIHR model
    Vessel circadian_rhythm();
    Vessel simple_catalytic(size_t A0, size_t B0, size_t C_count, double lambda = 0.001);


}
#endif //STOCHASTIC_H
