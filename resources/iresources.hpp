#pragma once

#include <memory>

namespace shell {

class IResource;

class IResources
{
public:
    virtual ~IResources() {}

    virtual void registerResource(std::weak_ptr<IResource> resource) = 0;
};

}
