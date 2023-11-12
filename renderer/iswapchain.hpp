#pragma once

#include <types.hpp>

#include <cstdint>
#include <functional>

class IRenderTarget;

class ISwapchain
{
public:
    struct CreateInfo
    {
        uint32_t framesInFlight = 2;
    };

public:
    virtual ~ISwapchain() {}

    virtual uint32_t width() const = 0;
    virtual uint32_t height() const = 0;
    virtual void setDrawCallback(std::function<void(IRenderTarget&)> callback) = 0;
    virtual void present() = 0;
};
