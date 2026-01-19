#include "factory.hpp"

template <typename Node>
void NodeCollection<Node>::add(Node&& node) {
    nodes_.push_back(std::move(node));
}

template <typename Node>
void NodeCollection<Node>::remove_by_id(ElementID id) {
    auto it = find_by_id(id);
    if (it != nodes_.end()) {
        nodes_.erase(it);
    }
}

template <typename Node>
typename NodeCollection<Node>::iterator
NodeCollection<Node>::find_by_id(ElementID id) {
    return std::find_if(nodes_.begin(), nodes_.end(),
        [id](const Node& n) { return n.get_id() == id; });
}

template <typename Node>
typename NodeCollection<Node>::const_iterator
NodeCollection<Node>::find_by_id(ElementID id) const {
    return std::find_if(nodes_.cbegin(), nodes_.cend(),
        [id](const Node& n) { return n.get_id() == id; });
}

template <typename Node>
typename NodeCollection<Node>::iterator
NodeCollection<Node>::begin() {
    return nodes_.begin();
}

template <typename Node>
typename NodeCollection<Node>::iterator
NodeCollection<Node>::end() {
    return nodes_.end();
}

template <typename Node>
typename NodeCollection<Node>::const_iterator
NodeCollection<Node>::begin() const {
    return nodes_.begin();
}

template <typename Node>
typename NodeCollection<Node>::const_iterator
NodeCollection<Node>::end() const {
    return nodes_.end();
}

template <typename Node>
typename NodeCollection<Node>::const_iterator
NodeCollection<Node>::cbegin() const {
    return nodes_.cbegin();
}

template <typename Node>
typename NodeCollection<Node>::const_iterator
NodeCollection<Node>::cend() const {
    return nodes_.cend();
}

// =======================================================
// Factory – dodawanie / usuwanie
// =======================================================

void Factory::add_ramp(Ramp&& ramp) {
    ramps_.add(std::move(ramp));
}

void Factory::add_worker(Worker&& worker) {
    workers_.add(std::move(worker));
}

void Factory::add_storehouse(Storehouse&& storehouse) {
    storehouses_.add(std::move(storehouse));
}

void Factory::remove_ramp(ElementID id) {
    ramps_.remove_by_id(id);
}

void Factory::remove_worker(ElementID id) {
    remove_receiver(workers_, id);
    workers_.remove_by_id(id);
}

void Factory::remove_storehouse(ElementID id) {
    remove_receiver(storehouses_, id);
    storehouses_.remove_by_id(id);
}

// =======================================================
// Dostęp do kolekcji
// =======================================================

NodeCollection<Ramp>::const_iterator
Factory::ramp_cbegin() const { return ramps_.cbegin(); }

NodeCollection<Ramp>::const_iterator
Factory::ramp_cend() const { return ramps_.cend(); }

NodeCollection<Worker>::const_iterator
Factory::worker_cbegin() const { return workers_.cbegin(); }

NodeCollection<Worker>::const_iterator
Factory::worker_cend() const { return workers_.cend(); }

NodeCollection<Storehouse>::const_iterator
Factory::storehouse_cbegin() const { return storehouses_.cbegin(); }

NodeCollection<Storehouse>::const_iterator
Factory::storehouse_cend() const { return storehouses_.cend(); }

// =======================================================
// Usuwanie odbiorcy z preferencji
// =======================================================

// template <typename Node>
// void Factory::remove_receiver(NodeCollection<Node>& collection, ElementID id) {
//     for (auto& worker : workers_) {
//         auto& prefs = receiver_preferences;
//         for (auto it = prefs.begin(); it != prefs.end(); ) {
//             if ((*it).first->get_id() == id) {
//                 prefs.remove_receiver((*it).first);
//                 it = prefs.begin();
//             } else {
//                 ++it;
//             }
//         }
//     }

//     for (auto& ramp : ramps_) {
//         auto& prefs = receiver_preferences;
//         for (auto it = prefs.begin(); it != prefs.end(); ) {
//             if ((*it).first->get_id() == id) {
//                 prefs.remove_receiver((*it).first);
//                 it = prefs.begin();
//             } else {
//                 ++it;
//             }
//         }
//     }
// }

template <typename Node>
void Factory::remove_receiver(NodeCollection<Node>& collection, ElementID id) {

    // USUŃ odbiorcę z preferencji WORKERÓW
    for (auto& worker : workers_) {
        auto& prefs = worker.receiver_preferences;

        for (auto it = prefs.begin(); it != prefs.end(); ) {
            if (it->first->get_id() == id) {
                prefs.remove_receiver(it->first);
                it = prefs.begin();
            } else {
                ++it;
            }
        }
    }

    // USUŃ odbiorcę z preferencji RAMP
    for (auto& ramp : ramps_) {
        auto& prefs = ramp.receiver_preferences;

        for (auto it = prefs.begin(); it != prefs.end(); ) {
            if (it->first->get_id() == id) {
                prefs.remove_receiver(it->first);
                it = prefs.begin();
            } else {
                ++it;
            }
        }
    }
}

// =======================================================
// Spójność sieci (DFS)
// =======================================================

bool Factory::is_consistent() const {
    std::map<const PackageSender*, bool> visited;

    for (auto it = ramps_.cbegin(); it != ramps_.cend(); ++it) {
        if (!has_reachable_storehouse(&(*it), visited)) {
            return false;
        }
    }
    return true;
}

bool Factory::has_reachable_storehouse(
    const PackageSender* sender,
    std::map<const PackageSender*, bool>& visited
) const {
    if (visited[sender]) {
        return false;
    }
    visited[sender] = true;

    const auto& prefs = sender->receiver_preferences.get_preferences();
    if (prefs.empty()) {
        throw std::logic_error("Sender has no receivers");
    }

    for (const auto& kv : prefs) {
        IPackageReceiver* receiver = kv.first;

        if (receiver->get_receiver_type() == ReceiverType::STOREHOUSE) {
            return true;
        }

        if (receiver->get_receiver_type() == ReceiverType::WORKER) {
            const Worker* worker =
                dynamic_cast<const Worker*>(receiver);

            if (worker) {
                const PackageSender* next = worker;
                if (has_reachable_storehouse(next, visited)) {
                    return true;
                }
            }
        }
    }

    return false;
}

// Etapy symulacji

// 1️⃣ Dostawy na rampach
void Factory::do_deliveries(Time t) {
    for (auto it = ramps_.begin(); it != ramps_.end(); ++it) {
        it->deliver_goods(t);
    }
}

// 2️⃣ Przekazywanie paczek (nadawcy -> odbiorcy)
void Factory::do_package_passing() {
    for (auto it = ramps_.begin(); it != ramps_.end(); ++it) {
        it->send_package();
    }

    for (auto it = workers_.begin(); it != workers_.end(); ++it) {
        it->send_package();
    }
}

// 3️⃣ Praca robotników
void Factory::do_work(Time t) {
    for (auto it = workers_.begin(); it != workers_.end(); ++it) {
        it->do_work(t);
    }
}

// // ===== kolory DFS =====
// enum class NodeColor { UNVISITED, VISITED, VERIFIED };

// // ===== DFS =====
// static bool has_reachable_storehouse(
//     const PackageSender* sender,
//     std::map<const PackageSender*, NodeColor>& colors
// ) {
//     auto& color = colors[sender];

//     if (color == NodeColor::VERIFIED) return true;

//     color = NodeColor::VISITED;

//     const auto& prefs = sender->receiver_preferences();
//     if (prefs.empty()) {
//         throw std::logic_error("Sender has no receivers");
//     }

//     bool has_valid_receiver = false;

//     for (auto it = prefs.cbegin(); it != prefs.cend(); ++it) {
//         IPackageReceiver* receiver = *it;

//         if (receiver->get_receiver_type() == ReceiverType::STOREHOUSE) {
//             has_valid_receiver = true;
//             continue;
//         }

//         auto worker = dynamic_cast<Worker*>(receiver);
//         auto next = dynamic_cast<PackageSender*>(worker);

//         if (!worker || !next) {
//             throw std::logic_error("Invalid WORKER hierarchy");
//         }

//         if (next == sender) continue;

//         has_valid_receiver = true;

//         if (colors[next] == NodeColor::UNVISITED) {
//             has_reachable_storehouse(next, colors);
//         }
//     }

//     color = NodeColor::VERIFIED;

//     if (!has_valid_receiver) {
//         throw std::logic_error("No path to storehouse");
//     }

//     return true;
// }
