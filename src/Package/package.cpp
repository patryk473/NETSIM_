#include "package.hpp"

#include <set>
#include <stdexcept>

std::set<ElementID> Package::assigned_ids_{};
std::set<ElementID> Package::freed_ids_{};
ElementID Package::max_id_{0};

Package::Package()
    : id_(allocate_id()), valid_(true) {}

Package::Package(ElementID forced_id)
    : id_(forced_id), valid_(true) {

    freed_ids_.erase(forced_id);

    if (assigned_ids_.count(forced_id) != 0) {
        throw std::logic_error("Package: ID already assigned");
    }

    assigned_ids_.insert(forced_id);
    if (forced_id > max_id_) {
        max_id_ = forced_id;
    }
}

Package::Package(Package&& other) noexcept
    : id_(other.id_), valid_(other.valid_) {

    other.id_ = 0;
    other.valid_ = false;
}

Package& Package::operator=(Package&& other) noexcept {
    if (this == &other) return *this;

    release_id_if_valid();

    id_ = other.id_;
    valid_ = other.valid_;

    other.id_ = 0;
    other.valid_ = false;

    return *this;
}

ElementID Package::get_id() const {
    if (!valid_) {
        throw std::logic_error("Package: accessing moved-from object");
    }
    return id_;
}

Package::~Package() {
    release_id_if_valid();
}

ElementID Package::allocate_id() {
    if (!freed_ids_.empty()) {
        auto it = freed_ids_.begin();
        ElementID id = *it;
        freed_ids_.erase(it);
        assigned_ids_.insert(id);
        return id;
    }

    ElementID new_id = max_id_ + 1;
    max_id_ = new_id;
    assigned_ids_.insert(new_id);
    return new_id;
}

void Package::release_id_if_valid() {
    if (!valid_) return;

    auto it = assigned_ids_.find(id_);
    if (it != assigned_ids_.end()) {
        assigned_ids_.erase(it);
        freed_ids_.insert(id_);
    }

    valid_ = false;
    id_ = 0;
}
