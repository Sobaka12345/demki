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

    class Descriptor
    {
    public:
        virtual void bind(OperationContext& context);
        virtual void setBinding(uint32_t bindingId, std::shared_ptr<IShaderResource> resource);
        std::shared_ptr<IShaderResource> binding(uint32_t bindingId);

    private:
        std::map<uint32_t, std::shared_ptr<IShaderResource>> m_resources;
    };

    class ObjectPool
    {
    public:
        class Factory : public IContextObject
        {
        public:
            virtual IGraphicsContext& context() final override { return pipeline().context(); }

            template <typename ObjectPoolT>
            std::shared_ptr<ObjectPoolT> spawn(uint32_t poolSize)
            {
                auto descriptor = pipeline().spawnDescriptor();
                populateDescriptor(*descriptor, poolSize);
                return std::shared_ptr<ObjectPoolT>{ ObjectPool::create<ObjectPoolT>(descriptor,
                    poolSize) };
            }

        protected:
            virtual void populateDescriptor(Descriptor& descriptor, uint32_t poolSize) = 0;

            IPipeline& pipeline() { return *m_pipeline; }

        private:
            template <typename FactoryT>
            static FactoryT* create(IPipeline* p)
            {
                auto result = new FactoryT;
                result->m_pipeline = p;
                return result;
            }
            friend class IPipeline;

        private:
            IPipeline* m_pipeline = nullptr;
        };
        friend class Factory;

    public:
        void bind(OperationContext& context) { m_descriptor->bind(context); }

    protected:
        ObjectPool() = default;

        void setBinding(uint32_t bindingId, std::shared_ptr<IShaderResource> resource)
        {
            m_descriptor->setBinding(bindingId, resource);
        }

        template <typename T>
        T& binding(uint32_t bindingId)
        {
            return static_cast<T&>(*m_descriptor->binding(bindingId).get());
        }

        template <typename T>
        const T& binding(uint32_t bindingId) const
        {
            return static_cast<const T&>(*m_descriptor->binding(bindingId).get());
        }

        uint32_t size() const { return m_size; }

        virtual void init() {};

    private:
        template <typename ObjectPoolT>
        static ObjectPoolT* create(std::shared_ptr<Descriptor> descriptor, uint32_t size)
        {
            auto result = new ObjectPoolT;
            result->m_descriptor = descriptor;
            result->m_size = size;
            result->init();
            return result;
        }

    private:
        std::shared_ptr<Descriptor> m_descriptor;
        uint32_t m_size;
    };

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

    virtual std::shared_ptr<IPipeline::Descriptor> spawnDescriptor() = 0;

    template <typename ObjectPoolFactoryT>
    std::shared_ptr<ObjectPoolFactoryT> createPoolFactory()
    {
        return std::shared_ptr<ObjectPoolFactoryT>{
            ObjectPoolFactoryT::template create<ObjectPoolFactoryT>(this)
        };
    }

    virtual ~IPipeline() {}
};

}    //  namespace renderer
