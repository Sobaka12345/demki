#pragma once

#include "types.hpp"
#include "shader_interface.hpp"
#include "vertex.hpp"

#include <filesystem>
#include <memory>
#include <variant>
#include <vector>
#include <span>


class RenderContext;
class IShaderInterfaceContainer;
class IShaderInterfaceHandle;

class IPipeline
{
public:
    typedef std::variant<Vertex3DColoredTextured, Vertex3DColored, Vertex3D> InputType;
    typedef ShaderStage ShaderType;

    enum Type
    {
        GRAPHICS,
        COMPUTE
    };

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
        template <IsShaderInterfaceContainer T>
        CreateInfo& addShaderInterfaceContainer()
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

        CreateInfo& type(Type value)
        {
            m_type = value;
            return *this;
        }

        const auto& type() const { return m_type; }

        auto& type() { return m_type; }

    private:
        Type m_type = GRAPHICS;
        SampleShading m_sampleShading = SampleShading::SS_0_PERCENT;
        std::vector<InputType> m_inputs;
        std::vector<ShaderInfo> m_shaders;
        std::vector<std::pair<uint32_t, std::span<const ShaderInterfaceBinding>>>
            m_uniformContainers;
    };

    struct IBindContext
    {
        virtual void bind(RenderContext& context, const IShaderInterfaceContainer& container) = 0;

        ~IBindContext() {}
    };

public:
    virtual void bind(RenderContext& context) = 0;
    virtual std::weak_ptr<IBindContext> bindContext(const IShaderInterfaceContainer& container) = 0;

    virtual ~IPipeline() {}
};
