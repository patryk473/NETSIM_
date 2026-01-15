#pragma once

#include <cstdint>

using ElementID = std::uint32_t;

// Zgodnie z instrukcją: typ odbiorcy potrzebny do sprawdzania spójności.
enum class ReceiverType { WORKER, STOREHOUSE };

// Interfejs odbiorcy paczek musi udostępniać get_receiver_type()
class IPackageReceiver {
public:
    virtual ~IPackageReceiver() = default;
    virtual ReceiverType get_receiver_type() const = 0;
    virtual ElementID get_id() const = 0;   // przydatne do usuwania powiązań
};
