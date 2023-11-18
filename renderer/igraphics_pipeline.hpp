#pragma once

#include <ipipeline.hpp>

class IGraphicsPipeline : virtual public IPipeline
{
public:
    class CreateInfo : public IPipeline::CreateInfo<CreateInfo>
    {
    public:
        CreateInfo& addInput(InputType input)
        {
            m_inputs.push_back(input);
            return *this;
        }

        const auto& inputs() const { return m_inputs; }

        auto& inputs() { return m_inputs; }

        CreateInfo& sampleShading(SampleShading value)
        {
            m_sampleShading = value;
            return *this;
        }

        const auto& sampleShading() const { return m_sampleShading; }

        auto& sampleShading() { return m_sampleShading; }

    private:
        SampleShading m_sampleShading = SampleShading::SS_0_PERCENT;
        std::vector<InputType> m_inputs;
    };
};
