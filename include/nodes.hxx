#ifndef NODES_HPP_
#define NODES_HPP_


#include <memory> 
#include "types.hxx"
#include "storage_types.hxx"

enum class ReceiverType {
    WORKER,
    STOREHOUSE
};

class IPackageReceiver {
    public:
        virtual void receive_package(Package&& p) = 0;
        virtual ElementID get_id() const = 0;
        virtual ~IPackageReceiver() = default;
};

class Storehouse: public IPackageReceiver, public IPackageStockpile {
    public:
        Storehouse(ElementID id, std::unique_ptr<IPackageReceiver> d);
    private:
        ElementID id_;
        std::unique_ptr<IPackageReceiver> d_;
};

class ReceiverPreferences {
    public:
        ReceiverPreferences(ProbabilityGenerator pg = probability_generator);
        using preferences_t = std::map<IPackageReceiver*, double>;
        
        void add_receiver(IPackageReceiver* r);
        void remove_receiver(IPackageReceiver* r);
        IPackageReceiver* choose_receiver();

        preferences_t& get_preferences() const;
    private:
        ProbabilityGenerator pg_;
};

class PackageSender {
    public:
        PackageSender(PackageSender&&) = default;
        
        ReceiverPreferences receiver_preferences; //jakas wartosc?

        void send_package();
        std::optional<Package>& get_sending_buffer() const;
    protected:
        void push_package(Package&&);
    private:
        std::optional<Package> sb_ = std::nullopt;
};

class Ramp: public PackageSender {
    public:
        Ramp(ElementID id, TimeOffset di);

        void deliver_goods(Time t);
        TimeOffset get_delivery_interval() const;
        ElementID get_id() const;
    private:
        ElementID id_;
        TimeOffset di_;
};

class Worker: public PackageSender, public IPackageReceiver, public IPackageQueue {
    public:
        Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q);

        void do_work(Time t);
        TimeOffset get_processing_duration() const;
        Time get_package_processing_start_time() const;
    private:
        ElementID id_;
        TimeOffset pd_;
        std::unique_ptr<IPackageQueue> q_;
};


#endif /* NODES_HPP_ */