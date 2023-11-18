#pragma once

#include "types.hpp"
#include "shader_interface.hpp"
#include "vertex.hpp"

#include <filesystem>
#include <memory>
#include <variant>
#include <vector>
#include <span>


class OperationContext;
class OperationContext;
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

    struct IBindContext
    {
        virtual void bind(OperationContext& context,
            const IShaderInterfaceContainer& container) = 0;

        ~IBindContext() {}
    };

protected:
    template <typename Derived>
    class CreateInfo
    {
    public:
        template <IsShaderInterfaceContainer T>
        Derived& addShaderInterfaceContainer()
        {
            m_interfaceContainers.push_back({ T::sId(), T::sLayout() });
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
        std::vector<std::pair<uint32_t, std::span<const ShaderInterfaceBinding>>>
            m_interfaceContainers;
    };


public:
    virtual void bind(OperationContext& context) = 0;
    virtual std::weak_ptr<IBindContext> bindContext(const IShaderInterfaceContainer& container) = 0;

    virtual ~IPipeline() {}
};
