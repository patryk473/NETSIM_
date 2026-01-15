#pragma once
#include "receiver_preferences.hpp"

class Ramp : public PackageSender {
public:
    explicit Ramp(ElementID id) : id_(id) {}

    ElementID get_id() const override { return id_; }
    const ReceiverPreferencesView& receiver_preferences() const override { return prefs_; }
    void remove_receiver(IPackageReceiver* receiver) override { prefs_.remove_receiver(receiver); }

    // helper
    void add_receiver(IPackageReceiver* receiver) { prefs_.add_receiver(receiver); }

private:
    ElementID id_;
    ReceiverPreferences prefs_;
};
