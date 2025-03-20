#pragma once

namespace renderer {

class IGraphicsContext;

class IContextObject {
public:
    virtual IGraphicsContext& context() = 0;

    virtual ~IContextObject() {};
};

}
