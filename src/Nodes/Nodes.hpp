#pragma once
#include <map>
#include <optional>
#include <functional>
#include <list>
#include <memory>
#include <stdexcept>

#include "package/Package.hpp"

using Time = int;
using TimeOffset = int;

enum class ReceiverType { WORKER, STOREHOUSE };

class IPackageReceiver {
public:
    virtual ~IPackageReceiver() = default;
    virtual void receive_package(Package&& p) = 0;
    virtual ElementID get_id() const = 0;
    virtual ReceiverType get_receiver_type() const = 0;
};

// Random generator
using ProbabilityGenerator = std::function<double()>;

// Receiver preferences
class ReceiverPreferences {
public:
    using preferences_t = std::map<IPackageReceiver*, double>;

    explicit ReceiverPreferences(ProbabilityGenerator pg = nullptr);

    void add_receiver(IPackageReceiver* r);
    void remove_receiver(IPackageReceiver* r);
    IPackageReceiver* choose_receiver() const;

    const preferences_t& get_preferences() const { return prefs_; }
    bool empty() const { return prefs_.empty(); }

private:
    void normalize();

    preferences_t prefs_;
    ProbabilityGenerator pg_;
};

// Sender base
class PackageSender {
public:
    ReceiverPreferences receiver_preferences;

    void push_package(Package&& p);
    void send_package();

protected:
    std::optional<Package> sending_buffer_;
};

// LoadingRamp
class LoadingRamp : public PackageSender {
public:
    LoadingRamp(ElementID id, TimeOffset di);

    ElementID get_id() const { return id_; }
    TimeOffset get_delivery_interval() const { return di_; }

    void deliver_goods(Time t);

private:
    ElementID id_;
    TimeOffset di_;
};

// Worker
class Worker : public PackageSender, public IPackageReceiver {
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q);

    // receiver
    void receive_package(Package&& p) override;
    ElementID get_id() const override { return id_; }
    ReceiverType get_receiver_type() const override { return ReceiverType::WORKER; }

    // work
    void do_work(Time t);

    // raportowe gettery
    std::optional<ElementID> pbuffer_id() const;
    int pbuffer_pt(Time t) const;
    std::optional<ElementID> sbuffer_id() const;

    const IPackageQueue* get_queue() const { return q_.get(); }

private:
    ElementID id_;
    TimeOffset pd_;

    std::unique_ptr<IPackageQueue> q_;
    std::optional<Package> pbuf_;
    Time start_t_ = 0;
};

// Storehouse
class Storehouse : public IPackageReceiver {
public:
    explicit Storehouse(ElementID id);

    void receive_package(Package&& p) override;
    ElementID get_id() const override { return id_; }
    ReceiverType get_receiver_type() const override { return ReceiverType::STOREHOUSE; }

    using const_iterator = std::list<Package>::const_iterator;
    const_iterator begin() const { return stock_.begin(); }
    const_iterator end() const { return stock_.end(); }
    bool empty() const { return stock_.empty(); }

private:
    ElementID id_;
    std::list<Package> stock_;
};
