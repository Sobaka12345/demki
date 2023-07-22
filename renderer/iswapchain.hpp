#pragma once

#include "irender_target.hpp"

#include <cstdint>
#include <functional>

class ISwapchain : public IRenderTarget
{
public:
    struct CreateInfo
    {};

public:
    virtual ~ISwapchain() {}

    virtual void setDrawCallback(std::function<void(IRenderTarget&)> callback) = 0;
    virtual void present() = 0;

    virtual uint32_t width() const = 0;
    virtual uint32_t height() const = 0;
};
