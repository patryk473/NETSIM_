#include "Nodes.hpp"

// =======================================================
// ReceiverPreferences
// =======================================================

ReceiverPreferences::ReceiverPreferences(ProbabilityGenerator pg)
    : probability_generator_(pg ? pg : [](){ return 0.5; }) {}

void ReceiverPreferences::normalize() {
    if (preferences_.empty()) return;

    double p = 1.0 / preferences_.size();
    for (auto& kv : preferences_) {
        kv.second = p;
    }
}

void ReceiverPreferences::add_receiver(IPackageReceiver* receiver) {
    preferences_[receiver] = 0.0;
    normalize();
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* receiver) {
    preferences_.erase(receiver);
    normalize();
}

IPackageReceiver* ReceiverPreferences::choose_receiver() {
    if (preferences_.empty() || !probability_generator_) {
        return nullptr;
    }

    double p = probability_generator_();
    double sum = 0.0;

    for (auto& kv : preferences_) {
        sum += kv.second;
        if (p <= sum) {
            return kv.first;
        }
    }
    return nullptr;
}

const ReceiverPreferences::preferences_t&
ReceiverPreferences::get_preferences() const {
    return preferences_;
}

ReceiverPreferences::const_iterator ReceiverPreferences::begin() const {
    return preferences_.begin();
}

ReceiverPreferences::const_iterator ReceiverPreferences::end() const {
    return preferences_.end();
}

ReceiverPreferences::const_iterator ReceiverPreferences::cbegin() const {
    return preferences_.cbegin();
}

ReceiverPreferences::const_iterator ReceiverPreferences::cend() const {
    return preferences_.cend();
}

bool ReceiverPreferences::empty() const {
    return preferences_.empty();
}

// =======================================================
// PackageSender
// =======================================================

PackageSender::PackageSender()
    : has_sending_package_(false) {}

void PackageSender::push_package(Package&& package) {
    sending_package_ = std::move(package);
    has_sending_package_ = true;
}

void PackageSender::send_package() {
    if (!has_sending_package_) return;

    IPackageReceiver* receiver =
        receiver_preferences.choose_receiver();

    if (receiver) {
        receiver->receive_package(std::move(sending_package_));
        has_sending_package_ = false;
    }
}

bool PackageSender::has_package() const {
    return has_sending_package_;
}

const Package& PackageSender::get_package() const {
    return sending_package_;
}

// =======================================================
// Ramp
// =======================================================

Ramp::Ramp(ElementID id, TimeOffset delivery_interval)
    : id_(id), delivery_interval_(delivery_interval) {}

void Ramp::deliver_goods(Time t) {
    if ((t - 1) % delivery_interval_ == 0) {
        push_package(Package());
    }
}

ElementID Ramp::get_id() const {
    return id_;
}

TimeOffset Ramp::get_delivery_interval() const {
    return delivery_interval_;
}

// =======================================================
// Worker
// =======================================================

Worker::Worker(
    ElementID id,
    TimeOffset processing_duration,
    std::unique_ptr<IPackageQueue> queue
)
    : id_(id),
      processing_duration_(processing_duration),
      queue_(std::move(queue)),
      is_processing_(false),
      processing_start_time_(0) {}

void Worker::receive_package(Package&& package) {
    queue_->push(std::move(package));
}

void Worker::do_work(Time t) {
    if (!is_processing_ && !queue_->empty()) {
        processing_package_ = queue_->pop();
        processing_start_time_ = t;
        is_processing_ = true;
    }

    if (is_processing_) {
        if (t - processing_start_time_ + 1 >= processing_duration_) {
            push_package(std::move(processing_package_));
            is_processing_ = false;
        }
    }
}

ElementID Worker::get_id() const {
    return id_;
}

ReceiverType Worker::get_receiver_type() const {
    return ReceiverType::WORKER;
}

TimeOffset Worker::get_processing_duration() const {
    return processing_duration_;
}

Time Worker::get_package_processing_start_time() const {
    return processing_start_time_;
}

bool Worker::is_processing() const {
    return is_processing_;
}

IPackageQueue* Worker::get_queue() const {
    return queue_.get();
}

Worker::const_iterator Worker::begin() const {
    return queue_->begin();
}

Worker::const_iterator Worker::end() const {
    return queue_->end();
}

Worker::const_iterator Worker::cbegin() const {
    return queue_->cbegin();
}

Worker::const_iterator Worker::cend() const {
    return queue_->cend();
}

// =======================================================
// Storehouse
// =======================================================

Storehouse::Storehouse(
    ElementID id,
    std::unique_ptr<IPackageStockpile> stockpile
)
    : id_(id), stockpile_(std::move(stockpile)) {}

void Storehouse::receive_package(Package&& package) {
    stockpile_->push(std::move(package));
}

ElementID Storehouse::get_id() const {
    return id_;
}

ReceiverType Storehouse::get_receiver_type() const {
    return ReceiverType::STOREHOUSE;
}

Storehouse::const_iterator Storehouse::begin() const {
    return stockpile_->begin();
}

Storehouse::const_iterator Storehouse::end() const {
    return stockpile_->end();
}

Storehouse::const_iterator Storehouse::cbegin() const {
    return stockpile_->cbegin();
}

Storehouse::const_iterator Storehouse::cend() const {
    return stockpile_->cend();
}
