 #pragma once

#include <types.hpp>

class IWindow {
public:
    virtual ~IWindow(){}
private:
    virtual void init(GAPI gapi) = 0;

    friend class IGraphicsContext;
};
