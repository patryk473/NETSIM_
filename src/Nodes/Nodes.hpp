// Nodes -> Definicja węzłów sieci produkcyjnej
// {
// Ramp -> źródło półproduktów
// Worker -> przetwarzanie półproduktów
// Storehouse -> ujście półproduktów
// PackageSender -> wspólna funkcjonalność nadawcy (rampa + robotnik)

#pragma once

#include <map>
//#include <optional>
#include <functional>
#include <memory>
// #include <stdexcept>
// #include <list>

#include "package/Package.hpp"

//Alias typu czasu symulacji
using Time = int;
using TimeOffset = int;

//Alias generatora liczb losowych
using ProbabilityGenerator = std::function<double()>;


//Typ odbiorcy
enum class ReceiverType { WORKER, STOREHOUSE };

//Abstrakcyjny odbiorca produktów
class IPackageReceiver {
public:
    using const_iterator = IPackageStockpile::const_iterator;
    virtual ~IPackageReceiver() = default;
    virtual void receive_package(Package&& p) = 0;

    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;

    virtual ElementID get_id() const = 0;
    virtual ReceiverType get_receiver_type() const = 0;
};



// Receiver preferences
class ReceiverPreferences {
public:
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;

    explicit ReceiverPreferences(
        ProbabilityGenerator pg = ProbabilityGenerator()
    );

    void add_receiver(IPackageReceiver* r);
    void remove_receiver(IPackageReceiver* r);

    IPackageReceiver* choose_receiver();

    const preferences_t& get_preferences() const;

    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    bool empty() const;

private:
    void normalize();

    preferences_t preferences_;
    ProbabilityGenerator probability_generator_;
};

// Sender base
class PackageSender {
public:
    ReceiverPreferences receiver_preferences;

    PackageSender();
    PackageSender(PackageSender&&) = default;

    PackageSender(const PackageSender&) = delete;
    PackageSender& operator=(const PackageSender&) = delete;

    virtual ~PackageSender() = default;

    void push_package(Package&& p);
    void send_package();

    bool has_package() const;
    const Package& get_package() const;

protected:
    bool has_sending_package_;
    Package sending_package_;
};

// LoadingRamp
class Ramp : public PackageSender {
public:
    Ramp(ElementID id, TimeOffset delivery_interval);

    ElementID get_id() const;
    TimeOffset get_delivery_interval() const;

    void deliver_goods(Time t);

private:
    ElementID id_;
    TimeOffset delivery_interval_;
};

// Worker
class Worker : public PackageSender, public IPackageReceiver {
public:
    Worker(
        ElementID id,
        TimeOffset processing_duration,
        std::unique_ptr<IPackageQueue> queue
    );

    Worker(const Worker&) = delete;
    Worker& operator=(const Worker&) = delete;

    Worker(Worker&&) = default;
    Worker& operator=(Worker&&) = default;

    // receiver
    void receive_package(Package&& p) override;
    ElementID get_id() const override;
    ReceiverType get_receiver_type() const override;

    // work
    void do_work(Time t);

    TimeOffset get_processing_duration() const;
    Time get_package_processing_start_time() const;

    bool is_processing() const;

    IPackageQueue* get_queue() const;

    const_iterator begin() const override;
    const_iterator end() const override;
    const_iterator cbegin() const override;
    const_iterator cend() const override;

    ~Worker() override = default;


private:
    ElementID id_;
    TimeOffset processing_duration_;

    std::unique_ptr<IPackageQueue> queue_;

    bool is_processing_;
    Package processing_package_;
    Time processing_start_time_;
};

// Storehouse
class Storehouse : public IPackageReceiver {
public:
    explicit Storehouse(
        ElementID id,
        std::unique_ptr<IPackageStockpile> stockpile =
            std::unique_ptr<IPackageStockpile>(
                new PackageQueue(PackageQueueType::FIFO))
    );

    Storehouse(const Storehouse&) = delete;
    Storehouse& operator=(const Storehouse&) = delete;

    Storehouse(Storehouse&&) = default;
    Storehouse& operator=(Storehouse&&) = default;

    void receive_package(Package&& p) override;
    ElementID get_id() const override;
    ReceiverType get_receiver_type() const override;

    const_iterator begin() const override;
    const_iterator end() const override;
    const_iterator cbegin() const override;
    const_iterator cend() const override;

    ~Storehouse() override = default;

private:
    ElementID id_;
    std::unique_ptr<IPackageStockpile> stockpile_;
};