#ifndef FACTORY_HPP_
#define FACTORY_HPP_

#include <list>
#include <map>
#include "helpers.hxx"
#include "nodes.hxx"

enum class NodeColor { UNVISITED, VISITED, VERIFIED };

template <class Node>
class NodeCollection {

    // Umieszczenie słowa kluczowego `typename` jest niezbędne aby poinformować
    // kompilator, że `Node` to nazwa typu.
    using container_t = typename std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    public:
        void add(Node&& node) { container_.push_back( std::move(node)); };
        void remove_by_id(ElementID id) {
            auto it = find_by_id(id);
            if (it != container_.end()) {container_.erase(it);}
        };
        iterator find_by_id(ElementID id) {
            return std::find_if(container_.begin(), container_.end(),
                [id](const auto& elem){ return (elem.get_id() == id); });
        };
        const_iterator find_by_id(ElementID id) const {
            return std::find_if(container_.begin(), container_.end(),
                [id](const auto& elem){ return (elem.get_id() == id); });
        };

        iterator begin() { return container_.begin(); }
        iterator end() { return container_.end(); }
        const_iterator begin() const { return container_.cbegin(); }
        const_iterator end() const { return container_.cend(); }
        const_iterator cbegin() const { return container_.cbegin(); }
        const_iterator cend() const { return container_.cend(); }

    private:
        container_t container_;

};


class Factory {

    public:
        void add_ramp(Ramp&& ramp) { return container_r_.add(std::move(ramp)); };
        void remove_ramp(ElementID id) { return container_r_.remove_by_id(id); };
        NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) { return container_r_.find_by_id(id); };
        NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const { return container_r_.find_by_id(id); };
        NodeCollection<Ramp>::const_iterator ramp_cbegin() const { return container_r_.cbegin(); };
        NodeCollection<Ramp>::const_iterator ramp_cend() const { return container_r_.cend(); };
        
        void add_worker(Worker&& worker) { return container_w_.add(std::move(worker)); };
        void remove_worker(ElementID id) { return container_w_.remove_by_id(id); };
        NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) { return container_w_.find_by_id(id); };
        NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const { return container_w_.find_by_id(id); };
        NodeCollection<Worker>::const_iterator worker_cbegin() const { return container_w_.cbegin(); };
        NodeCollection<Worker>::const_iterator worker_cend() const { return container_w_.cend(); };

        void add_storehouse(Storehouse&& storehouse) { return container_s_.add(std::move(storehouse)); };
        void remove_storehouse(ElementID id) { return container_s_.remove_by_id(id); };
        NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id) { return container_s_.find_by_id(id); };
        NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const { return container_s_.find_by_id(id); };
        NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const { return container_s_.cbegin(); };
        NodeCollection<Storehouse>::const_iterator storehouse_cend() const { return container_s_.cend(); };

        bool is_consistent();
        void do_deliveries(Time);
        void do_package_passing();
        void do_work(Time);

    private:
        void remove_receiver(NodeCollection<Node>& collection, ElementID id);

        NodeCollection<Ramp> container_r_;
        NodeCollection<Worker> container_w_;
        NodeCollection<Storehouse> container_s_;

};

#endif
