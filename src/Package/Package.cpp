#include "Package.hpp"
#include <stdexcept>

//Inicjalizacja statycznych członków klasy Package
std::set<ElementID> Package::assigned_ids_; //assigned - przypisane
std::set<ElementID> Package::freed_ids_; //freed - zwolnione

ElementID Package::generate_id() {
    if (!freed_ids_.empty()) {
        //Jeśli są zwolnione ID, użyj jednego z nich
        ElementID id = *freed_ids_.begin();
        freed_ids_.erase(freed_ids_.begin());
        assigned_ids_.insert(id);
        return id;
    } 

    //W przeciwnym razie wygeneruj nowe ID na zasadzie max + 1
    ElementID new_id = assigned_ids_.empty() ? 1 : (*assigned_ids_.rbegin() + 1);
    assigned_ids_.insert(new_id);
    return new_id;
}

//losowe ID
Package::Package() : id_(generate_id()) {}

//wybrane konkretne ID
Package::Package(ElementID id) : id_(id) {
    if (assigned_ids_.find(id) != assigned_ids_.end()) {
        throw std::invalid_argument("ID already assigned");
    }
    assigned_ids_.insert(id);
}

// konstruktor przenoszący
Package::Package(Package&& other) noexcept : id_(other.id_) {
    other.id_ = -1; //Unieważnij ID w obiekcie źródłowym
}

//operator przenoszący
Package& Package::operator=(Package&& other) noexcept {
    if (this != &other) {
        //Zwolnij obecne ID, jeśli jest ważne
        id_ = other.id_;
        other.id_ = -1; //Unieważnij ID w obiekcie źródłowym
    }
    return *this;
}

//destruktor
Package::~Package() {
    if (id_ != -1) {
        assigned_ids_.erase(id_);
        freed_ids_.insert(id_);
    }
}

//getter
ElementID Package::getID() const {
    return id_;
}

PackageQueue::PackageQueue(PackageQueueType type) : type_(type) {}

void PackageQueue::push(Package&& package) {
    container_.push_back(std::move(package));
}

Package PackageQueue::pop() {
    if (container_.empty()) {
        throw std::out_of_range("PackageQueue is empty");
    }

    Package result = std::move(
        (type_ == PackageQueueType::FIFO) ? container_.front() : container_.back()
    );
    if (type_ == PackageQueueType::FIFO) {
        container_.pop_front();
    } else {
        container_.pop_back();
    }

    return result;
}

bool PackageQueue::empty() const {
    return container_.empty();
}

std::size_t PackageQueue::size() const {
    return container_.size();
}

PackageQueue::const_iterator PackageQueue::begin() const {
    return container_.begin();
}

PackageQueue::const_iterator PackageQueue::end() const {
    return container_.end();
}

PackageQueue::const_iterator PackageQueue::cbegin() const {
    return container_.cbegin();
}

PackageQueue::const_iterator PackageQueue::cend() const {
    return container_.cend();
}

PackageQueueType PackageQueue::getQueueType() const {
    return type_;
}