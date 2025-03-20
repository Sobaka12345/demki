#pragma once

#include "icontext_object.hpp"
#include "ishader_interface.hpp"

#include <spirv_reflect.h>

#include <variant>
#include <memory>
#include <map>

namespace renderer {

class OperationContext;
class IShaderResource;

class IPipeline : public IContextObject
{
public:
    typedef std::variant<Vertex3DColoredTextured, Vertex3DColored, Vertex3D> InputType;

    struct Descriptor
    {
        virtual void bind(OperationContext& context);
        virtual void setBinding(uint32_t bindingId, std::shared_ptr<IShaderResource> resource);

        std::map<uint32_t, std::shared_ptr<IShaderResource>> resources;

    protected:
        Descriptor() = default;
    };

    class Object
    {
    public:
        Object(IPipeline& pipeline)
            : descriptor(pipeline.spawnDescriptor())
        {}

        virtual void init() = 0;

        void setBinding(uint32_t bindingId, std::shared_ptr<IShaderResource> resource)
        {
            descriptor->setBinding(bindingId, resource);
        }

        void bind(OperationContext& context) { return descriptor->bind(context); }

    private:
        std::shared_ptr<Descriptor> descriptor;
    };

    // template <typename PipelineObjectT>
    // std::shared_ptr<PipelineObjectT> createPipelineObjectFactory()
    // {
    //     auto result = std::make_shared<PipelineObjectT>(*this);

    //     return result;
    // }

protected:
    template <typename Derived>
    class CreateInfo
    {
    public:
        inline std::span<const std::shared_ptr<SpvReflectShaderModule>> shaderModules() const
        {
            return m_shaderModules;
        }

        inline Derived& addShaderModule(std::span<const unsigned char> shader)
        {
            auto module =
                std::shared_ptr<SpvReflectShaderModule>(new SpvReflectShaderModule, [](auto* p) {
                    spvReflectDestroyShaderModule(p);
                    delete p;
                });

            SpvReflectResult result =
                spvReflectCreateShaderModule(shader.size_bytes(), shader.data(), module.get());
            ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

            uint32_t count = 0;
            result = spvReflectEnumerateDescriptorSets(module.get(), &count, NULL);
            ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);
            std::vector<SpvReflectDescriptorSet*> sets(count);
            result = spvReflectEnumerateDescriptorSets(module.get(), &count, sets.data());
            ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

            result = spvReflectEnumerateDescriptorBindings(module.get(), &count, NULL);
            ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);
            std::vector<SpvReflectDescriptorBinding*> bindings(count);
            result = spvReflectEnumerateDescriptorBindings(module.get(), &count, bindings.data());
            ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

            result = spvReflectEnumerateInterfaceVariables(module.get(), &count, NULL);
            assert(result == SPV_REFLECT_RESULT_SUCCESS);
            std::vector<SpvReflectInterfaceVariable*> interface_variables(count);
            result = spvReflectEnumerateInterfaceVariables(module.get(), &count,
                interface_variables.data());
            assert(result == SPV_REFLECT_RESULT_SUCCESS);

            result = spvReflectEnumerateInputVariables(module.get(), &count, NULL);
            assert(result == SPV_REFLECT_RESULT_SUCCESS);
            std::vector<SpvReflectInterfaceVariable*> input_variables(count);
            result =
                spvReflectEnumerateInputVariables(module.get(), &count, input_variables.data());
            assert(result == SPV_REFLECT_RESULT_SUCCESS);
            result = spvReflectEnumerateOutputVariables(module.get(), &count, NULL);
            assert(result == SPV_REFLECT_RESULT_SUCCESS);
            std::vector<SpvReflectInterfaceVariable*> output_variables(count);
            result =
                spvReflectEnumerateOutputVariables(module.get(), &count, output_variables.data());
            assert(result == SPV_REFLECT_RESULT_SUCCESS);

            result = spvReflectEnumeratePushConstantBlocks(module.get(), &count, NULL);
            assert(result == SPV_REFLECT_RESULT_SUCCESS);
            std::vector<SpvReflectBlockVariable*> push_constant(count);
            result =
                spvReflectEnumeratePushConstantBlocks(module.get(), &count, push_constant.data());
            assert(result == SPV_REFLECT_RESULT_SUCCESS);

            m_shaderModules.push_back(module);

            return that();
        }

    private:
        Derived& that() { return *static_cast<Derived*>(this); }

    private:
        std::vector<std::shared_ptr<SpvReflectShaderModule>> m_shaderModules;
    };


public:
    virtual void bind(OperationContext& context) = 0;

    virtual ~IPipeline() {}

private:
    virtual std::shared_ptr<IPipeline::Descriptor> spawnDescriptor() = 0;
};

}    //  namespace renderer
