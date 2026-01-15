#include "factory.hpp"
#include <map>
#include <stdexcept>

// ===== kolory DFS =====
enum class NodeColor { UNVISITED, VISITED, VERIFIED };

// ===== DFS =====
static bool has_reachable_storehouse(
    const PackageSender* sender,
    std::map<const PackageSender*, NodeColor>& colors
) {
    auto& color = colors[sender];

    if (color == NodeColor::VERIFIED) return true;

    color = NodeColor::VISITED;

    const auto& prefs = sender->receiver_preferences();
    if (prefs.empty()) {
        throw std::logic_error("Sender has no receivers");
    }

    bool has_valid_receiver = false;

    for (auto it = prefs.cbegin(); it != prefs.cend(); ++it) {
        IPackageReceiver* receiver = *it;

        if (receiver->get_receiver_type() == ReceiverType::STOREHOUSE) {
            has_valid_receiver = true;
            continue;
        }

        auto worker = dynamic_cast<Worker*>(receiver);
        auto next = dynamic_cast<PackageSender*>(worker);

        if (!worker || !next) {
            throw std::logic_error("Invalid WORKER hierarchy");
        }

        if (next == sender) continue;

        has_valid_receiver = true;

        if (colors[next] == NodeColor::UNVISITED) {
            has_reachable_storehouse(next, colors);
        }
    }

    color = NodeColor::VERIFIED;

    if (!has_valid_receiver) {
        throw std::logic_error("No path to storehouse");
    }

    return true;
}
