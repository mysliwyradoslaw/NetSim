#include "storage_types.hxx"

PackageQueue::PackageQueue(PackageQueueType type) : queue_type(type) {};

void PackageQueue::push(Package&& item) {
    queue.emplace_back(std::move(item));
}

Package PackageQueue::pop() {
    Package pkg;

    switch (queue_type) {
        case PackageQueueType::FIFO:
            pkg = std::move(queue.back());
            queue.pop_back();

        case PackageQueueType::LIFO:
            pkg = std::move(queue.front());
            queue.pop_front();
    }

    return pkg;
};