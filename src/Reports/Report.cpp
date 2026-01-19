#include "Report.hpp"

// =======================================================
// Raport struktury sieci
// =======================================================

void Reports::print_factory_structure(const Factory& factory, std::ostream& os) {

    os << "== FACTORY STRUCTURE ==\n\n";

    // ----------------------
    // RAMP
    // ----------------------
    os << "== RAMPS ==\n";
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        os << "RAMP id=" << it->get_id()
           << " delivery-interval=" << it->get_delivery_interval()
           << "\n";
    }
    os << "\n";

    // ----------------------
    // WORKER
    // ----------------------
    os << "== WORKERS ==\n";
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        os << "WORKER id=" << it->get_id()
           << " processing-time=" << it->get_processing_duration()
           << " queue-type="
           << (it->get_queue()->getQueueType() == PackageQueueType::FIFO ? "FIFO" : "LIFO")
           << "\n";
    }
    os << "\n";

    // ----------------------
    // STOREHOUSE
    // ----------------------
    os << "== STOREHOUSES ==\n";
    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        os << "STOREHOUSE id=" << it->get_id() << "\n";
    }
    os << "\n";

    // ----------------------
    // LINKS
    // ----------------------
    os << "== LINKS ==\n";

    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        for (auto pref = it->receiver_preferences.begin();
             pref != it->receiver_preferences.end(); ++pref) {

            os << "RAMP " << it->get_id()
               << " -> " << pref->first->get_id()
               << "\n";
        }
    }

    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        for (auto pref = it->receiver_preferences.begin();
             pref != it->receiver_preferences.end(); ++pref) {

            os << "WORKER " << it->get_id()
               << " -> " << pref->first->get_id()
               << "\n";
        }
    }

    os << "\n";
}

// =======================================================
// Raport stanu symulacji
// =======================================================

void Reports::print_simulation_state(const Factory& factory, Time t, std::ostream& os) {

    os << "=== SIMULATION STATE (t=" << t << ") ===\n\n";

    // ----------------------
    // RAMPS
    // ----------------------
    os << "== RAMPS ==\n";
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        os << "RAMP id=" << it->get_id()
           << " buffer="
           << (it->has_package() ? "OCCUPIED" : "EMPTY")
           << "\n";
    }
    os << "\n";

    // ----------------------
    // WORKERS
    // ----------------------
    os << "== WORKERS ==\n";
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {

        os << "WORKER id=" << it->get_id() << "\n";

        os << "  processing: "
           << (it->is_processing() ? "YES" : "NO")
           << "\n";

        os << "  queue: ";
        for (auto qit = it->cbegin(); qit != it->cend(); ++qit) {
            os << qit->getID() << " ";
        }
        os << "\n";
    }
    os << "\n";

    // ----------------------
    // STOREHOUSES
    // ----------------------
    os << "== STOREHOUSES ==\n";
    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        os << "STOREHOUSE id=" << it->get_id() << " stockpile: ";
        for (auto sit = it->cbegin(); sit != it->cend(); ++sit) {
            os << sit->getID() << " ";
        }
        os << "\n";
    }

    os << "\n";
}
