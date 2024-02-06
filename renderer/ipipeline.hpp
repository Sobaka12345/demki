#pragma once

#include "shader_interface.hpp"
#include "vertex.hpp"

#include <utils.hpp>

#include <filesystem>
#include <memory>
#include <variant>
#include <vector>
#include <span>
#include <type_utils.hpp>

class OperationContext;
class OperationContext;
class IShaderInterfaceContainer;
class IShaderInterfaceHandle;

struct IPipelineBindContext
{
    virtual void bind(OperationContext& context, const IShaderInterfaceContainer& container) = 0;

    virtual ~IPipelineBindContext() {}
};

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

protected:
    struct InterfaceContainerInfo
    {
        uint32_t id = 0;
        uint32_t batchSize = 1;
        std::span<const ShaderInterfaceBinding> layout;
    };

    template <typename Derived>
    class CreateInfo
    {
    public:
        template <IsShaderInterfaceContainer T>
        Derived& addShaderInterfaceContainer(uint32_t batchSize = 1)
        {
            m_interfaceContainers.push_back({ T::sId(), batchSize, T::sLayout() });
            return that();
        }

        const auto& interfaceContainers() const { return m_interfaceContainers; }

        auto& interfaceContainers() { return m_interfaceContainers; }

        Derived& addShader(ShaderInfo shaderInfo)
        {
            m_shaders.push_back(shaderInfo);
            return that();
        }

        const auto& shaders() const { return m_shaders; }

        auto& shaders() { return m_shaders; }

    private:
        Derived& that() { return *static_cast<Derived*>(this); }

    private:
        std::vector<ShaderInfo> m_shaders;
        std::vector<InterfaceContainerInfo> m_interfaceContainers;
    };


public:
    virtual void bind(OperationContext& context) = 0;
    virtual FragileSharedPtr<IPipelineBindContext> bindContext(
        const IShaderInterfaceContainer& container) = 0;

    virtual ~IPipeline() {}
};
