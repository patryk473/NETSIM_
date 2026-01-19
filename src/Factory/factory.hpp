#pragma once
#include <list>
#include <map>
#include <stdexcept>
#include <algorithm>

#include "Nodes/Nodes.hpp"

template <typename Node>
class NodeCollection {
public:
    using container_t = std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    // Dodaje węzeł do kolekcji (przeniesienie własności)
    void add(Node&& node);

    // Usuwa węzeł o danym ID
    void remove_by_id(ElementID id);

    // Wyszukuje węzeł po ID
    iterator find_by_id(ElementID id);
    const_iterator find_by_id(ElementID id) const;

    // Iteratory
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

private:
    container_t nodes_;
};

class Factory {
public:
    // --- API fabryki ---
    void add_ramp(Ramp&& r);
    void add_worker(Worker&& w);
    void add_storehouse(Storehouse&& s);

    void remove_ramp(ElementID id);
    void remove_worker(ElementID id);
    void remove_storehouse(ElementID id);

    NodeCollection<Ramp>::const_iterator ramp_cbegin() const;
    NodeCollection<Ramp>::const_iterator ramp_cend() const;

    NodeCollection<Worker>::const_iterator worker_cbegin() const;
    NodeCollection<Worker>::const_iterator worker_cend() const;

    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const;
    NodeCollection<Storehouse>::const_iterator storehouse_cend() const;
    bool is_consistent() const;

    // „hooki” z UML (puste na tym etapie)
    void do_deliveries(Time);
    void do_package_passing();
    void do_work(Time);

private:
    NodeCollection<Ramp> ramps_;
    NodeCollection<Worker> workers_;
    NodeCollection<Storehouse> storehouses_;

    // prywatna metoda pomocnicza
    template <typename Node>
    void remove_receiver(NodeCollection<Node>& collection, ElementID id);
    bool has_reachable_storehouse(
    const PackageSender* sender,
    std::map<const PackageSender*, bool>& visited
    ) const;
};
