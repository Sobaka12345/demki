#pragma once

#include <cstdint>
#include <crtp.hpp>

#include <set>

namespace resources {

struct Resource {
    using ResourceTrackerT = std::set<void*>;
    using ResourceTrackerIdT = ResourceTrackerT::const_iterator;
    
    static ResourceTrackerT s_resources;
    
    ResourceTrackerIdT resourceId;

    //static uint32_t s_typeCounter;
};

template <typename ConcreteResource>
struct SIResource : protected CRTPBase<ConcreteResource>
{
    // SIResource() noexcept 
    // {
    //     // static bool _ = [this]() {
    //     //     const_cast<SIResource*>(this)->s_typeId = ++Resource::s_typeCounter;
    //     // };

    //     Resource::resourceId = Resource::s_resources.insert(this).first;
    // }

    // ~SIResource() noexcept 
    // {
    //     Resource::s_resources.erase(Resource::resourceId);
    // }

    //static const uint32_t s_typeId = 0;
};

}    //  namespace resources
