#include "shader_interface.hpp"

#include <operation_context.hpp>

#include <ipipeline.hpp>

uint32_t IShaderInterfaceContainer::s_id = 0;

IShaderInterfaceContainer::~IShaderInterfaceContainer()
{
}

void IShaderInterfaceContainer::bind(OperationContext& context)
{
    IPipeline* pipeline = &context.pipeline();

    if (auto iter = m_contexts.find(pipeline); iter == m_contexts.end())
    {
        auto [newEl, _] = 
            m_contexts.emplace(pipeline, pipeline->bindContext(*this));
        newEl->second->bind(context, *this);
    }
    else
    {
        iter->second->bind(context, *this);
    }
}

uint32_t IShaderInterfaceContainer::createId()
{
    return s_id++;
}
