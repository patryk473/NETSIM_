#pragma once
#include "node_collection.hpp"
#include "ramp.hpp"
#include "worker.hpp"
#include "storehouse.hpp"

class Factory {
public:
    // --- API fabryki ---
    void add_ramp(Ramp&& r);
    void add_worker(Worker&& w);
    void add_storehouse(Storehouse&& s);

    void remove_ramp(ElementID id);
    void remove_worker(ElementID id);
    void remove_storehouse(ElementID id);

    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id);
    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id);
    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id);

    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const;
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const;
    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const;

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
};
