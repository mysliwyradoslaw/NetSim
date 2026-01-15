#include "package.hxx"

static std::set<ElementID> assigned_IDs = {};
static std::set<ElementID> freed_IDs = {};

Package::Package() {
    if (!freed_IDs.empty()) {
        id = *freed_IDs.cbegin();
        freed_IDs.erase(freed_IDs.cbegin());
    }
    else if (!assigned_IDs.empty()) {
        id = *(prev(assigned_IDs.cend())) + 1;
    }
    else {
        id = 1;
    }

    assigned_IDs.insert(id);
}

Package::~Package() {
    assigned_IDs.erase(id);
    freed_IDs.insert(id);
}