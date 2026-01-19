#pragma once

// ==============================
// Simulation.hpp
// ==============================
// Warstwa symulacji
//
// Odpowiada za:
// - uruchomienie symulacji na zadany czas
// - wywoływanie kolejnych etapów Factory
// - delegowanie raportowania (wzorzec Strategia)
//
// Zgodne z PDF „Symulacja”
// ==============================

#include <functional>

#include "Factory/factory.hpp"

// =======================================================
// Funkcja symulacji
// =======================================================
//
// f   -> obiekt Factory (struktura sieci)
// d   -> liczba tur symulacji
// rf  -> funkcja raportująca (Strategy)
//
// Funkcja NIE przechowuje stanu pomiędzy wywołaniami
//
void simulate(
    Factory& f,
    TimeOffset d,
    std::function<void(Factory&, Time)> rf
);
