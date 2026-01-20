#ifndef NODES_HPP_
#define NODES_HPP_


#include <memory>
#include <map>
#include <optional>

#include "types.hxx"
#include "storage_types.hxx"
#include "package.hxx"
#include "helpers.hxx"

enum class ReceiverType {
    WORKER,
    STOREHOUSE
};

class IPackageReceiver {
    public:
        virtual void receive_package(Package&& p) = 0;
        virtual ElementID get_id() const = 0;
        virtual ~IPackageReceiver() = default;

        using const_iterator = IPackageStockpile::const_iterator;

        virtual const_iterator begin() const = 0;
        virtual const_iterator end()   const = 0;

        virtual const_iterator cbegin() const = 0;
        virtual const_iterator cend()   const = 0;
};

class Storehouse: public IPackageReceiver {
    public:
        Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::LIFO));

        void receive_package(Package&& p) override { d_->push(std::move(p)); }
        ElementID get_id() const override { return id_; }

        const_iterator begin() const override { return d_->begin(); }
        const_iterator end()   const override { return d_->end(); }

        const_iterator cbegin() const override { return d_->cbegin();}
        const_iterator cend()   const override { return d_->cend();}

    private:
        ElementID id_;
        std::unique_ptr<IPackageStockpile> d_;
};

class ReceiverPreferences {
    public:
        ReceiverPreferences(ProbabilityGenerator pg = probability_generator);

        using preferences_t = std::map<IPackageReceiver*, double>;
        using const_iterator = preferences_t::const_iterator;

        const_iterator begin() const { return preferences_.begin(); }
        const_iterator end()   const { return preferences_.end(); }

        const_iterator cbegin() const { return preferences_.cbegin(); }
        const_iterator cend()   const { return preferences_.cend(); }
        
        void add_receiver(IPackageReceiver* r);
        void remove_receiver(IPackageReceiver* r);
        IPackageReceiver* choose_receiver() const;

        const preferences_t& get_preferences() const {return preferences_;}
    private:
        void normalize_preferences();
        ProbabilityGenerator pg_;
        preferences_t preferences_;
};

class PackageSender {
    public:
        PackageSender(PackageSender&&) = default;
        
        ReceiverPreferences receiver_preferences_ = ReceiverPreferences();

        void send_package();
        const std::optional<Package>& get_sending_buffer() const { return sb_; }
    protected:
        PackageSender() = default;

        void push_package(Package&& p);
    private:
        std::optional<Package> sb_ = std::nullopt;
};

class Ramp: public PackageSender {
    public:
        Ramp(ElementID id, TimeOffset di);

        void deliver_goods(Time t);
        TimeOffset get_delivery_interval() const { return di_; }
        ElementID get_id() const { return id_; }
    private:
        ElementID id_;
        TimeOffset di_;
};

class Worker: public PackageSender, public IPackageReceiver {
    public:
        Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q);

        void do_work(Time t);
        TimeOffset get_processing_duration() const { return pd_; }
        Time get_package_processing_start_time() const { return processing_start_time_; }

        ElementID get_id() const override { return id_; }
        void receive_package(Package&& p) override { q_->push(std::move(p)); }

        const_iterator begin() const override { return q_->begin(); }
        const_iterator end()   const override { return q_->end(); }

        const_iterator cbegin() const override { return q_->cbegin();}
        const_iterator cend()   const override { return q_->cend();}
    private:
        ElementID id_;
        TimeOffset pd_;
        std::unique_ptr<IPackageQueue> q_;
        Time processing_start_time_;
        std::optional<Package> processing_buffer_ = std::nullopt;
};


#endif /* NODES_HPP_ */