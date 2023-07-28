#pragma once

#include "types.hpp"
#include "uniform.hpp"
#include "vertex.hpp"

#include <filesystem>
#include <memory>
#include <variant>
#include <vector>
#include <span>


class RenderContext;
class IUniformContainer;
class IUniformHandle;

class IPipeline
{
public:
    typedef std::variant<Vertex3DColoredTextured, Vertex3DColored, Vertex3D> InputType;
    typedef UniformStage ShaderType;

    enum InputBindingID
    {
        IB_ID_VERTICES = 0,
    };

    struct InputBinding
    {
        InputBindingID id;
    };

    struct ShaderInfo
    {
        ShaderType type;
        std::filesystem::path path;
    };

    class CreateInfo
    {
    public:
        template <IsUniformContainer T>
        CreateInfo& addUniformContainer()
        {
            m_uniformContainers.push_back({ T::sId(), T::sLayout() });
            return *this;
        }

        const auto& uniformContainers() const { return m_uniformContainers; }

        auto& uniformContainers() { return m_uniformContainers; }

        CreateInfo& addShader(ShaderInfo shaderInfo)
        {
            m_shaders.push_back(shaderInfo);
            return *this;
        }

        const auto& shaders() const { return m_shaders; }

        auto& shaders() { return m_shaders; }

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
        std::vector<ShaderInfo> m_shaders;
        std::vector<std::pair<uint32_t, std::span<const UniformBinding>>> m_uniformContainers;
    };

    struct IBindContext
    {
        virtual void bind(RenderContext& context, const IUniformContainer& container) = 0;

        ~IBindContext() {}
    };

public:
    virtual void bind(RenderContext& context) = 0;
    virtual std::weak_ptr<IBindContext> bindContext(const IUniformContainer& container) = 0;

    virtual ~IPipeline() {}
};
