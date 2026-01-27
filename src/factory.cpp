#include "factory.hxx"
#include "helpers.hxx"
#include "nodes.hxx"
#include "storage_types.hxx"
#include <memory>
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
        for (const auto& ramp : container_r_) {
            has_reachable_storehouse(&ramp, node_colors);
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

ParsedLineData parse_line(std::string line) {

    ParsedLineData parsed_line_data;

    size_t sep_pos = line.find(' ');
    std::string ID;
    
    if (sep_pos == std::string::npos) {
        throw std::invalid_argument("Wrong line format");
    }
    else {
        ID = line.substr(0, sep_pos);
        line.erase(0, sep_pos + 1);
    }

    if (ID == "LOADING_RAMP") {
        parsed_line_data.element_type = ElementType::RAMP;
    }
    else if (ID == "WORKER") {
        parsed_line_data.element_type = ElementType::WORKER;
    }
    else if (ID == "STOREHOUSE") {
        parsed_line_data.element_type = ElementType::STOREHOUSE;
    }
    else if (ID == "LINK") {
        parsed_line_data.element_type = ElementType::LINK;
    }
    else {
        throw std::invalid_argument("Wrong ID type: " + ID);
    }

    std::string param;
    std::istringstream param_stream(line);
    char delimiter = ' ';

    while (std::getline(param_stream, param, delimiter)) {
        sep_pos = param.find('=');

        parsed_line_data.parameters[param.substr(0, sep_pos)]
            = param.substr(sep_pos + 1, param.size() - sep_pos - 1);
    }

    return parsed_line_data;
}

Factory load_factory_structure(std::istream &is) {
    Factory factory;

    std::string line;
 
    while (std::getline(is, line)) {
        if (line.empty() || line[0] == ';') continue;

        ParsedLineData parsed_line_data = parse_line(line);

        try {

            switch (parsed_line_data.element_type) {

                case ElementType::RAMP:
                    factory.add_ramp(Ramp(std::stoi(parsed_line_data.parameters["id"]), std::stoi(parsed_line_data.parameters["delivery-interval"])));
                    break;

                case ElementType::WORKER: {
                    PackageQueueType q_type;

                    if (parsed_line_data.parameters["queue-type"] == "LIFO") {
                        q_type = PackageQueueType::LIFO;
                    }
                    else if (parsed_line_data.parameters["queue-type"] == "FIFO") {
                        q_type = PackageQueueType::FIFO;
                    }
                    else {
                        throw std::invalid_argument("Wrong queue type");
                    }

                    factory.add_worker(Worker(std::stoi(parsed_line_data.parameters["id"]),
                        std::stoi(parsed_line_data.parameters["processing-time"]), std::make_unique<PackageQueue>(q_type)));
                    break;
                }

                case ElementType::STOREHOUSE:
                    factory.add_storehouse(Storehouse(std::stoi(parsed_line_data.parameters["id"])));
                    break;

                case ElementType::LINK: {
                    std::string src_type, dest_type;
                    ElementID src_id = 0, dest_id = 0;

                    for (const auto& [param, val] : parsed_line_data.parameters) {
                        size_t sep_pos = val.find('-');

                        if (sep_pos == std::string::npos) {
                            throw std::invalid_argument("Wrong parameter format in LINK");
                        }

                        std::string type = val.substr(0, sep_pos);
                        ElementID id = std::stoi(val.substr(sep_pos + 1));

                        if (param == "src") {
                            src_type = type;
                            src_id = id;
                        } else if (param == "dest") {
                            dest_type = type;
                            dest_id = id;
                        }
                    }

                    IPackageReceiver* receiver_ptr = nullptr;
                    if (dest_type == "store") {
                        receiver_ptr = &(*factory.find_storehouse_by_id(dest_id));
                    } else if (dest_type == "worker") {
                        receiver_ptr = &(*factory.find_worker_by_id(dest_id));
                    }

                    if (src_type == "ramp") {
                        factory.find_ramp_by_id(src_id)->receiver_preferences_.add_receiver(receiver_ptr);
                    } else if (src_type == "worker") {
                        factory.find_worker_by_id(src_id)->receiver_preferences_.add_receiver(receiver_ptr);
                    }
                    break;
                }
            }
        }
        catch (int e) {
            throw e;
        }
    }

    return factory;
}

std::string queue_type_str(PackageQueueType package_queue_type) {
    switch(package_queue_type) {
        case PackageQueueType::FIFO:
            return "FIFO";
        case PackageQueueType::LIFO:
            return "LIFO";
    }
    return {};
}

void link_stream_fill(std::stringstream& link_stream, const PackageSender& package_sender, ElementID package_sender_id, std::string&& package_sender_name) {
    auto prefs = package_sender.receiver_preferences_.get_preferences();

    std::for_each(prefs.cbegin(), prefs.cend(), [&](const std::pair<IPackageReceiver*, double>& key_value) {
        link_stream << "LINK src=" << package_sender_name << "-" << package_sender_id << " ";
        const IPackageReceiver* package_receiver = key_value.first;
        ReceiverType receiver_type = package_receiver->get_receiver_type();

        std::string receiver_type_str = receiver_type == ReceiverType::WORKER ? "worker" : "store";

        link_stream << "dest=" << receiver_type_str << "-" << package_receiver->get_id() << '\n';
        std::cout << link_stream.str();
    });
}

void save_factory_structure(Factory& factory, std::ostream& os) {
    // Elementy w pliku występującą w następującej kolejności:
    // LOADING_RAMP, WORKER, STOREHOUSE, LINK.

    std::stringstream link_stream;

    std::for_each(factory.ramp_cbegin(), factory.ramp_cend(), [&](const Ramp& ramp) {
        ElementID ramp_id = ramp.get_id();
        os << "LOADING_RAMP id=" << ramp_id << ' '
           << "delivery-interval=" << ramp.get_delivery_interval() << '\n';

        link_stream_fill(link_stream, ramp, ramp_id, "ramp");
    });

    std::for_each(factory.worker_cbegin(), factory.worker_cend(), [&](const Worker& worker) {
        PackageQueueType queue_type = worker.get_queue()->get_queue_type();
        ElementID worker_id = worker.get_id();
        os << "WORKER id=" << worker_id << ' '
           << "processing-time=" << worker.get_processing_duration() << ' '
           << "queue-type=" << queue_type_str(queue_type) << '\n';

        link_stream_fill(link_stream, worker, worker_id, "worker");
    });

    std::for_each(factory.storehouse_cbegin(), factory.storehouse_cend(), [&](const Storehouse& storehouse) {
        os << "STOREHOUSE id=" << storehouse.get_id() << '\n';
    });

    os << link_stream.str();

    os.flush();
}


