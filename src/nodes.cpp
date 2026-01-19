#include "nodes.hxx"


ReceiverPreferences::ReceiverPreferences(ProbabilityGenerator pg) : pg_(pg) {}

void ReceiverPreferences::normalize_preferences() {
    if (preferences_.empty()) { return; }

    const double scale = std::accumulate(preferences_.cbegin(), preferences_.cend(), 0.0,
        [](double sum, const auto& kv) {return sum + kv.second;});

    if (scale != 1.0) {
        for (auto& [_, value] : preferences_) {value /= scale;} //TODO: preference sum can be 0, should handle that
    }
}


void ReceiverPreferences::add_receiver(IPackageReceiver* r) {
    if (preferences_.empty()) {
        preferences_.emplace(r,1.0);
        return;
    }

    if (preferences_.find(r) != preferences_.end()) { return; } //could use std::map::contains instead if c++20

    preferences_.emplace(r, pg_());

    normalize_preferences();
}

void ReceiverPreferences::remove_receiver(IPackageReceiver* r) {
    if (preferences_.empty() or preferences_.find(r) == preferences_.end()) { return; }

    preferences_.erase(r);

    normalize_preferences();
}

IPackageReceiver* ReceiverPreferences::choose_receiver() const {
    if (preferences_.empty()) { return nullptr; } //TODO:probably should throw an exception but shouldn't ever be empty

    const auto u = pg_();

    double cumulative = 0.0;
    for (const auto& [receiver, chance] : preferences_) {
        cumulative += chance;
        if (u <= cumulative) {
            return receiver;
        }
    }
    return nullptr; //TODO:probably should throw an exception
}

void PackageSender::send_package() {
    if (!sb_) { return; }

    IPackageReceiver* receiver = receiver_preferences.choose_receiver();
    if (receiver == nullptr) { return; } //TODO: should throw an exception

    receiver -> receive_package(std::move(*sb_));
    sb_.reset();
}

void PackageSender::push_package(Package&& p) {
    if (sb_) { return; } //TODO: should either throw an exception or communicate that buffer is full
    sb_ = std::move(p);
}

Ramp::Ramp(ElementID id, TimeOffset di) : PackageSender(), id_(id), di_(di) {}

void Ramp::deliver_goods(Time t) {
    if ((t-1) % di_ == 0) {push_package(Package());}
}

Worker::Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q) : id_(id), pd_(pd), q_(std::move(q)) {}

void Worker::do_work(Time t) {
    if (pd_ == 1) {
        push_package(std::move(q_->pop()));
        return;
    }

    if (!processing_buffer_) {
        processing_buffer_ = std::move(q_->pop());
        processing_start_time_ = t;
        return;
    }

    if (t + 1 - processing_start_time_ == pd_) {
        push_package(std::move(*processing_buffer_));
        processing_buffer_.reset(); //if sb_ is not empty, this package won't be pushed and will be lost, should never happen
    }
}

Storehouse::Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d) : id_(id), d_(std::move(d)) {}



