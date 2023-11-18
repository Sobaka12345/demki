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
    public:
        CreateInfo& setComputeDimensions(ComputeDimensions computeDimensions)
        {
            m_computeDimensions = std::move(computeDimensions);
            return *this;
        }

        const ComputeDimensions& computeDimensions() const { return m_computeDimensions; }

        ComputeDimensions& computeDimensions() { return m_computeDimensions; }

    private:
        ComputeDimensions m_computeDimensions;
    };

public:
    virtual ComputeDimensions computeDimensions() const = 0;
};
