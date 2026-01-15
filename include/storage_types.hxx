#ifndef STORAGE_TYPES_HPP_
#define STORAGE_TYPES_HPP_


#include <list>

enum class PackageQueueType {
    FIFO,
    LIFO
};

class IPackageStockpile {
    using const_iterator = std::list<Package>::const_iterator;
    
};

#endif