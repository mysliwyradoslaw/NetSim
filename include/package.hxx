#ifndef PACKAGE_HPP_
#define PACKAGE_HPP_

#include <set>
#include "types.hxx"

class Package {
    private:
        static std::set<ElementID> assigned_IDs;
        static std::set<ElementID> freed_IDs;
        ElementID id_;

    public:
        Package();
        Package(ElementID id);
        Package(Package&&) = default;
        ~Package();

        Package& operator=(Package&&);
        ElementID get_id() const {return id_;};
};

#endif