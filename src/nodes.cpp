#include "nodes.hxx"


ReceiverPreferences::ReceiverPreferences(ProbabilityGenerator pg) : pg_(pg) {}

void ReceiverPreferences::normalize_preferences() {
    if (preferences_.empty()) { return; }

    const double scale = std::accumulate(preferences_.cbegin(), preferences_.cend(), 0.0,
        [](double sum, const auto& kv) {return sum + kv.second;});

    if (scale != 1.0) {
        for (auto& [_, value] : preferences_) {value /= scale;} //TO DO: preference sum can be 0, should handle that
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
    if (preferences_.empty()) { return nullptr; }

    const auto u = pg_();

    double cumulative = 0.0;
    for (const auto& [receiver, chance] : preferences_) {
        cumulative += chance;
        if (u <= cumulative) {
            return receiver;
        }
    }
    return nullptr; //probably should throw an exception
}