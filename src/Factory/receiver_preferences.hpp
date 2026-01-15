#pragma once
#include <map>

class IPackageReceiver;

class ReceiverPreferences {
public:
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    bool empty() const { return prefs_.empty(); }
    const_iterator cbegin() const { return prefs_.cbegin(); }
    const_iterator cend() const { return prefs_.cend(); }

    void add_receiver(IPackageReceiver* r, double p = 1.0) { prefs_[r] = p; }
    void remove_receiver(IPackageReceiver* r) { prefs_.erase(r); }

private:
    preferences_t prefs_;
};

