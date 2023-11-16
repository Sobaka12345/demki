#pragma once

#include <irender_target.hpp>
#include <types.hpp>

#include <cstdint>
#include <functional>

class ISwapchain : public IRenderTarget
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
