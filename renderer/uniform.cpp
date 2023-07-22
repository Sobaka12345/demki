#include "uniform.hpp"

uint32_t IUniformContainer::s_id = 0;

uint32_t IUniformContainer::createId()
{
    return s_id++;
}
