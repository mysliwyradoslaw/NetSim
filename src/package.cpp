#include "package.hxx"

std::set<ElementID> Package::assigned_IDs = {};
std::set<ElementID> Package::freed_IDs = {};

Package::Package(ElementID id) : id_(id) {assigned_IDs.insert(id_);};

Package::Package() {
    if (!freed_IDs.empty()) {
        id_ = *freed_IDs.cbegin();
        freed_IDs.erase(freed_IDs.cbegin());
    }
    else if (!assigned_IDs.empty()) {
        id_ = *(prev(assigned_IDs.cend())) + 1;
    }
    else {
        id_ = 1;
    }

    assigned_IDs.insert(id_);
}

Package::~Package() {
    assigned_IDs.erase(id_);
    freed_IDs.insert(id_);
}

