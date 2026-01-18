#ifndef STORAGE_TYPES_HPP_
#define STORAGE_TYPES_HPP_

#include <list>
#include "package.hxx"

enum class PackageQueueType {
    FIFO,
    LIFO
};


class IPackageStockpile {
public:
    using size_type = std::size_t;
    using const_iterator = std::list<Package>::const_iterator;

    virtual void push(Package&& item) = 0;
    virtual bool empty() const = 0;
    virtual size_type size() const = 0;

    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;

    virtual ~IPackageStockpile() = default;
};


class IPackageQueue : public IPackageStockpile {
public:
    virtual Package pop() = 0;
    virtual PackageQueueType get_queue_type() const = 0;
};

class PackageQueue : public IPackageQueue {
public:
    void push(Package&& item) override;
    bool empty() const override {return queue.empty();};
    size_type size() const override {return queue.size();};

    const_iterator begin() const override {return queue.begin();};
    const_iterator end() const override {return queue.end();};
    const_iterator cbegin() const override {return queue.cbegin();};
    const_iterator cend() const override {return queue.cend();};

    Package pop() override;
    PackageQueueType get_queue_type() const override {return queue_type;};

    PackageQueue(PackageQueueType type);

private:
    std::list<Package> queue;
    PackageQueueType queue_type;
};

#endif