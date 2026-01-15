#include "nodes/Nodes.hpp"
#include <random>

static std::mt19937& rng() {
    static std::mt19937 gen{ std::random_device{}() };
    return gen;
}
static double default_pg() { return std::generate_canonical<double, 16>(rng()); }

// ReceiverPreferences
ReceiverPreferences::ReceiverPreferences(ProbabilityGenerator pg)
    : pg_(pg ? std::move(pg) : ProbabilityGenerator(default_pg)) {}

void ReceiverPreferences::normalize() {
    if (prefs_.empty()) return;
    double p = 1.0 / static_cast<double>(prefs_.size());
    for (auto& kv : prefs_) kv.second = p;
}

void ReceiverPreferences::add_receiver(IPackageReceiver* r) {
    if (!r) return;
    prefs_[r] = 1.0;
    normalize();
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* r) {
    prefs_.erase(r);
    normalize();
}

IPackageReceiver* ReceiverPreferences::choose_receiver() const {
    if (prefs_.empty()) throw std::logic_error("No receivers");
    double p = pg_();
    double cum = 0.0;
    for (auto& [recv, prob] : prefs_) {
        cum += prob;
        if (p < cum) return recv;
    }
    return prefs_.rbegin()->first;
}

// PackageSender
void PackageSender::push_package(Package&& p) {
    if (sending_buffer_) throw std::logic_error("Sending buffer not empty");
    sending_buffer_.emplace(std::move(p));
}

void PackageSender::send_package() {
    if (!sending_buffer_) return;
    auto* r = receiver_preferences.choose_receiver();
    r->receive_package(std::move(*sending_buffer_));
    sending_buffer_.reset();
}

// LoadingRamp
LoadingRamp::LoadingRamp(ElementID id, TimeOffset di) : id_(id), di_(di) {
    if (di_ <= 0) throw std::logic_error("delivery interval must be > 0");
}

void LoadingRamp::deliver_goods(Time t) {
    if ((t - 1) % di_ == 0) {
        if (!sending_buffer_) push_package(Package{});
    }
}

// Worker
Worker::Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q)
    : id_(id), pd_(pd), q_(std::move(q)) {
    if (pd_ <= 0) throw std::logic_error("processing duration must be > 0");
    if (!q_) throw std::logic_error("queue null");
}

void Worker::receive_package(Package&& p) {
    q_->push(std::move(p));
}

void Worker::do_work(Time t) {
    if (!pbuf_) {
        if (q_->empty()) return;
        pbuf_.emplace(q_->pop());
        start_t_ = t;
    }
    int spent = t - start_t_ + 1;
    if (spent >= pd_) {
        push_package(std::move(*pbuf_));
        pbuf_.reset();
    }
}

std::optional<ElementID> Worker::pbuffer_id() const {
    if (!pbuf_) return std::nullopt;
    return pbuf_->getID();
}
int Worker::pbuffer_pt(Time t) const {
    if (!pbuf_) return 0;
    return t - start_t_ + 1;
}
std::optional<ElementID> Worker::sbuffer_id() const {
    if (!sending_buffer_) return std::nullopt;
    return sending_buffer_->getID();
}

// Storehouse
Storehouse::Storehouse(ElementID id) : id_(id) {}

void Storehouse::receive_package(Package&& p) {
    stock_.push_back(std::move(p));
}
