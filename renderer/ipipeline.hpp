#pragma once

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

    struct CreateInfo
    {
        template <IsUniformContainer T>
        CreateInfo& addUniformContainer()
        {
            uniformSets.push_back({ T::sId(), T::sLayout() });
            return *this;
        }

        CreateInfo& addShader(ShaderInfo shaderInfo)
        {
            shaders.push_back(shaderInfo);
            return *this;
        }

        CreateInfo& addInput(InputType input)
        {
            inputs.push_back(input);
            return *this;
        }

        std::vector<InputType> inputs;
        std::vector<ShaderInfo> shaders;
        std::vector<std::pair<uint32_t, std::span<const UniformBinding>>> uniformSets;
    };

    struct IBindContext
    {
        virtual void bind(RenderContext& context, const IUniformContainer& container) = 0;

        ~IBindContext() {}
    };

public:
    virtual void bind(RenderContext& context) = 0;
    virtual std::shared_ptr<IBindContext> bindContext(const IUniformContainer& container) = 0;

    virtual ~IPipeline() {}
};
