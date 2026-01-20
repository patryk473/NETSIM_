#include "Report.hpp"

// mały helper do powtarzalnych separatorów
static void print_header(std::ostream& os, const std::string& title) {
    os << "=========================\n"
       << "      " << title << "\n"
       << "=========================\n\n";
}

static void print_section(std::ostream& os, const std::string& title) {
    os << "[" << title << "]\n";
}

static const char* queue_type_to_str(PackageQueueType qt) {
    return (qt == PackageQueueType::FIFO) ? "FIFO" : "LIFO";
}

// =======================================================
// Raport struktury sieci (Opcja 3)
// =======================================================

void Reports::print_factory_structure(const Factory& factory, std::ostream& os) {

    print_header(os, "FACTORY REPORT");

    // ----------------------
    // RAMPS
    // ----------------------
    print_section(os, "Ramps");
    bool any = false;
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        any = true;
        os << "  • Ramp " << it->get_id()
           << " (delivery every " << it->get_delivery_interval() << ")\n";
    }
    if (!any) os << "  (none)\n";
    os << "\n";

    // ----------------------
    // WORKERS
    // ----------------------
    print_section(os, "Workers");
    any = false;
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        any = true;
        os << "  • Worker " << it->get_id()
           << " | time: " << it->get_processing_duration()
           << " | queue: " << queue_type_to_str(it->get_queue()->getQueueType())
           << "\n";
    }
    if (!any) os << "  (none)\n";
    os << "\n";

    // ----------------------
    // STOREHOUSES
    // ----------------------
    print_section(os, "Storehouses");
    any = false;
    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        any = true;
        os << "  • Storehouse " << it->get_id() << "\n";
    }
    if (!any) os << "  (none)\n";
    os << "\n";

    // ----------------------
    // LINKS
    // ----------------------
    print_section(os, "Links");

    // Ramps -> receivers
    any = false;
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        for (auto pref = it->receiver_preferences.begin();
             pref != it->receiver_preferences.end(); ++pref) {
            any = true;
            os << "  • Ramp " << it->get_id()
               << " -> " << pref->first->get_id()
               << "\n";
        }
    }

    // Workers -> receivers
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        for (auto pref = it->receiver_preferences.begin();
             pref != it->receiver_preferences.end(); ++pref) {
            any = true;
            os << "  • Worker " << it->get_id()
               << " -> " << pref->first->get_id()
               << "\n";
        }
    }

    if (!any) os << "  (none)\n";
    os << "\n";
}

// =======================================================
// Raport stanu symulacji (Opcja 3)
// =======================================================

void Reports::print_simulation_state(const Factory& factory, Time t, std::ostream& os) {

    os << "=========================\n"
       << "  SIMULATION STATE (t=" << t << ")\n"
       << "=========================\n\n";

    // ----------------------
    // RAMPS
    // ----------------------
    print_section(os, "Ramps");
    bool any = false;
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        any = true;
        os << "  • Ramp " << it->get_id()
           << " | buffer: " << (it->has_package() ? "OCCUPIED" : "EMPTY")
           << "\n";
    }
    if (!any) os << "  (none)\n";
    os << "\n";

    // ----------------------
    // WORKERS
    // ----------------------
    print_section(os, "Workers");
    any = false;
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        any = true;

        os << "  • Worker " << it->get_id() << "\n";
        os << "      processing : " << (it->is_processing() ? "YES" : "NO") << "\n";
        os << "      queue      : ";

        bool empty_q = true;
        for (auto qit = it->cbegin(); qit != it->cend(); ++qit) {
            empty_q = false;
            os << qit->getID() << " ";
        }
        if (empty_q) os << "(empty)";
        os << "\n";
    }
    if (!any) os << "  (none)\n";
    os << "\n";

    // ----------------------
    // STOREHOUSES
    // ----------------------
    print_section(os, "Storehouses");
    any = false;
    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        any = true;

        os << "  • Storehouse " << it->get_id() << "\n";
        os << "      stockpile  : ";

        bool empty_s = true;
        for (auto sit = it->cbegin(); sit != it->cend(); ++sit) {
            empty_s = false;
            os << sit->getID() << " ";
        }
        if (empty_s) os << "(empty)";
        os << "\n";
    }
    if (!any) os << "  (none)\n";
    os << "\n";
}
