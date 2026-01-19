#include "factory.hxx"
#include "nodes.hxx"
#include <stdexcept>


bool Factory::has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors) {

    if (node_colors[sender] == NodeColor::VERIFIED) {
        return true;
    }

    node_colors[sender] = NodeColor::VISITED;

    auto& preferences = sender->receiver_preferences_.get_preferences();

    if (preferences.empty()) {
        throw std::logic_error("No reachable storehouses found");
    }

    bool has_reachable_receiver_other_than_himself = false;

    for (const auto& [receiver, probability] : preferences) {
        if (receiver->get_receiver_type() == ReceiverType::STOREHOUSE) {
            has_reachable_receiver_other_than_himself = true;
        } else if (receiver->get_receiver_type() == ReceiverType::WORKER) {
            
            auto sendrecv_ptr = dynamic_cast<PackageSender*>(dynamic_cast<Worker*>(receiver));

            if (sendrecv_ptr == sender) {
                continue;
            }
            has_reachable_receiver_other_than_himself = true;

            if (node_colors[sendrecv_ptr] == NodeColor::UNVISITED) {
                has_reachable_storehouse(sendrecv_ptr, node_colors);
            }
        }
    }

    node_colors[sender] = NodeColor::VERIFIED;

    if (has_reachable_receiver_other_than_himself) {
        return true;
    }
    throw std::logic_error("No reachable storehouses found");
};

bool Factory::is_consistent() {
    std::map<const PackageSender*, NodeColor> node_colors;
    for (const auto& ramp : container_r_) {
        node_colors[&ramp] = NodeColor::UNVISITED;
    }
    for (const auto& worker : container_w_) {
        node_colors[&worker] = NodeColor::UNVISITED;
    }
    try {
        for (const auto& it = container_r_.cbegin(); it != container_r_.cend(); ++it) {
            has_reachable_storehouse(*it, node_colors);
        }
    }
    catch (std::logic_error&) {
        return false;
    }
    return true;
};

void Factory::do_deliveries(Time t) {
    for (auto& ramp : container_r_) {
        ramp.deliver_goods(t);
    }
}

void Factory::do_package_passing() {
    for (auto& ramp : container_r_) {
        ramp.send_package();
    }
    for (auto& worker : container_w_) {
        worker.send_package();
    }
};

void Factory::do_work(Time t) {
    for (auto& worker : container_w_) {
        worker.do_work(t);
    }
}


