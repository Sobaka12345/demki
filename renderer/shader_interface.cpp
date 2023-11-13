#include "shader_interface.hpp"

uint32_t IShaderInterfaceContainer::s_id = 0;

uint32_t IShaderInterfaceContainer::createId()
{
    return s_id++;
}
