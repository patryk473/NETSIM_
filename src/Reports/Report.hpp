#pragma once

// ==============================
// Report.hpp
// ==============================
// Warstwa prezentacji danych
//
// Odpowiada za:
// - raport struktury sieci
// - raport stanu symulacji
//
// Zgodne z PDF „Warstwa prezentacji danych”
// ==============================

#include <ostream>

#include "factory/Factory.hpp"

// =======================================================
// Namespace Reports
// =======================================================

namespace Reports {

    // Raport struktury sieci (topologia)
    void print_factory_structure(const Factory& factory, std::ostream& os);

    // Raport stanu symulacji w danej turze
    void print_simulation_state(const Factory& factory, Time t, std::ostream& os);

}
