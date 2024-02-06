#pragma once

#include <ipipeline.hpp>

class IComputePipeline : virtual public IPipeline
{
public:
    struct ComputeDimensions
    {
        uint32_t x = 1;
        uint32_t y = 1;
        uint32_t z = 1;
    };

    class CreateInfo : public IPipeline::CreateInfo<CreateInfo>
    {
        CREATE_INFO_PROPERTY(ComputeDimensions, computeDimensions, ComputeDimensions{});
    };

public:
    virtual ComputeDimensions computeDimensions() const = 0;
};
