#pragma once

#include <irender_target.hpp>
#include <icontext_object.hpp>
#include <types.hpp>

#include <cstdint>

namespace renderer {

class ISwapchain : public IRenderTarget, public IContextObject
{
public:
    struct CreateInfo
    {
        uint32_t framesInFlight = 2;
    };

public:
    virtual ~ISwapchain() {}

    virtual uint32_t framesInFlight() const = 0;
};

}    //  namespace renderer
