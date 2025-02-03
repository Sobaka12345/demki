#pragma once

#include "../vertex.hpp"

#include "icontext_object.hpp"
#include "ishader_interface.hpp"

#include <spirv_reflect.h>

#include <filesystem>
#include <variant>

namespace renderer {

class OperationContext;
class IShaderInterfaceContainer;
class IShaderInterfaceHandle;

class IPipeline : public IContextObject
{
public:
    typedef std::variant<Vertex3DColoredTextured, Vertex3DColored, Vertex3D> InputType;

    enum class Type
    {
        GRAPHICS = 0,
        COMPUTE,
        COUNT,
        INVALID
    };

    struct ShaderInfo {

    };

protected:
    template <typename Derived>
    class CreateInfo
    {

    public:
        inline Derived& addStage(ShaderStage stage, std::filesystem::path path)
        {
            auto& shaderInfo = m_shaders[enumT(stage)];


            return that();
        }

    private:
        Derived& that() { return *static_cast<Derived*>(this); }

    private:
        std::array<ShaderInfo, enumT(ShaderStage::COUNT)> m_shaders;
    };


public:
    virtual void bind(OperationContext& context) = 0;

    virtual ~IPipeline() {}
};

}    //  namespace renderer
