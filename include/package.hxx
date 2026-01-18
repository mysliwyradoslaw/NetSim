#ifndef PACKAGE_HPP_
#define PACKAGE_HPP_

#include <set>
#include "types.hxx"

class Package {
    private:
        static std::set<ElementID> assigned_IDs;
        static std::set<ElementID> freed_IDs;
        ElementID id;

    public:
        Package(ElementID ID = 0);
        ~Package();
};

#endif