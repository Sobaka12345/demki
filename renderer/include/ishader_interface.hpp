#pragma once

#include <cstdint>
#include <type_list.hpp>

#include <span>

namespace renderer {

enum class ShaderBlockType : uint16_t
{
    BEGIN = 0,
    UNIFORM_STATIC = BEGIN,
    UNIFORM_DYNAMIC,
    SAMPLER,
    STORAGE,
    COUNT,
    INVALID
};

enum class ShaderStage : uint16_t
{
    BEGIN = 0,
    COMPUTE,
    TASK,
    VERTEX,
    MESH,
    GEOMETRY,
    FRAGMENT,
    COUNT,
    INVALID
};

struct ShaderInterfaceBinding
{
    int16_t count = 1;
    int16_t size = 1;
    ShaderBlockType type = ShaderBlockType::INVALID;
    ShaderStage stage = ShaderStage::INVALID;
};

template <typename BindingTypeT,
    ShaderBlockType blockTypeArg,
    ShaderStage stageArg,
    uint16_t countArg = 1>
struct ShaderInterfaceBindingMeta
{
    using BindingType = BindingTypeT;

    enum : uint16_t
    {
        type = static_cast<uint16_t>(blockTypeArg),
        stage = static_cast<uint16_t>(stageArg),
        count = static_cast<uint16_t>(countArg),
        size = static_cast<uint16_t>(sizeof(BindingType))
    };
};

}    //  namespace renderer
