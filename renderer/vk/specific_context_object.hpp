#pragma once

#include "../utils.hpp"
#include "graphics_context.hpp"

#include <icontext_object.hpp>

namespace renderer::vk {

class ISpecificContextObject
{
public:
    ISpecificContextObject(GraphicsContext& context)
        : m_context(context)
    {}

protected:
    ~ISpecificContextObject() {};

    const GraphicsContext& context() const { return m_context; };

    virtual IContextObject* toBase() = 0;

protected:
    GraphicsContext& m_context;
};

template <typename IBase>
using SpecificContextObject = SpecificBase<IBase, ISpecificContextObject>;

}    //  namespace renderer::vk
