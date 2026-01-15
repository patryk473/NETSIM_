#pragma once
#include "receiver_preferences.hpp"

class Worker : public PackageSender, public IPackageReceiver {
public:
    explicit Worker(ElementID id) : id_(id) {}

    
    ReceiverType get_receiver_type() const override { return ReceiverType::WORKER; }
    ElementID get_id() const override { return id_; }

    
    const ReceiverPreferencesView& receiver_preferences() const override { return prefs_; }
    void remove_receiver(IPackageReceiver* receiver) override { prefs_.remove_receiver(receiver); }


    void add_receiver(IPackageReceiver* receiver) { prefs_.add_receiver(receiver); }

private:
    ElementID id_;
    ReceiverPreferences prefs_;
};
