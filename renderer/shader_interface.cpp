#include "../include/ishader_interface_container.hpp"

#include <operation_context.hpp>

#include <ipipeline.hpp>

namespace renderer {

uint32_t IShaderInterfaceContainer::s_id = 0;

uint32_t IShaderInterfaceContainer::createId()
{
    return s_id++;
}

}    //  namespace renderer
