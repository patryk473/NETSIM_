#include "Simulation.hpp"

// =======================================================
// Funkcja simulate()
// =======================================================

void simulate(
    Factory& f,
    TimeOffset d,
    std::function<void(Factory&, Time)> rf
) {
    // Sprawdzenie spójności sieci przed startem
    if (!f.is_consistent()) {
        throw std::logic_error("Factory network is not consistent");
    }

    // Pętla czasowa symulacji
    for (Time t = 1; t <= d; ++t) {

        // 1️⃣ Dostawy na rampy
        f.do_deliveries(t);

        // 2️⃣ Przekazywanie paczek (nadawcy -> odbiorcy)
        f.do_package_passing();

        // 3️⃣ Praca robotników
        f.do_work(t);

        // 4️⃣ Raportowanie (jeśli strategia tak zdecyduje)
        rf(f, t);
    }
}
