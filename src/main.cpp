#include <fstream>
#include <iostream>
#include "io/Parser.hpp"
#include "Simulation/Simulation.hpp"
#include "Reports/Report.hpp"

int main() {
    std::cout << "START\n";

    std::ifstream file("factory.txt");
    if (!file) {
        std::cout << "Nie mozna otworzyc factory.txt\n";
        return 1;
    }

    Factory factory = IO::load_factory_structure(file);

    std::cout << "\n--- STRUKTURA FABRYKI ---\n";
    Reports::print_factory_structure(factory, std::cout);

    std::cout << "\n--- SYMULACJA ---\n";

    simulate(factory, 6, [](Factory& f, Time t) {
        Reports::print_simulation_state(f, t, std::cout);
    });

    std::cout << "\nKONIEC\n";
    return 0;
}
