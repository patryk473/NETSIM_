#pragma once

#include <cstdint>

using ElementID = std::uint32_t;

class Package {
public:
    Package();
    explicit Package(ElementID id);

    Package(Package&& other) noexcept;
    Package& operator=(Package&& other) noexcept;

    Package(const Package&) = delete;
    Package& operator=(const Package&) = delete;

    ElementID get_id() const;

    ~Package();

private:
    ElementID id_{0};
    bool valid_{false};

    static ElementID allocate_id();
    void release_id_if_valid();

    static std::set<ElementID> assigned_ids_;
    static std::set<ElementID> freed_ids_;
    static ElementID max_id_;
};
