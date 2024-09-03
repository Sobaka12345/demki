#pragma once

#include <type_list.hpp>

#include <array>
#include <cstddef>

namespace renderer {

enum class ShaderBlockType : int16_t
{
    INVALID = -1,
    UNIFORM_STATIC = 0,
    UNIFORM_DYNAMIC,
    SAMPLER,
    STORAGE
};

enum class ShaderStage : int16_t
{
    INVALID = -1,
    VERTEX,
    FRAGMENT,
    COMPUTE
};

struct ShaderInterfaceBinding
{
    int16_t count = 1;
    ShaderBlockType type = ShaderBlockType::INVALID;
    ShaderStage stage = ShaderStage::INVALID;
};

template <size_t BindingCount = 1>
using ShaderInterfaceLayout = std::array<ShaderInterfaceBinding, BindingCount>;

template <typename BindingTypeT,
    ShaderBlockType blockTypeArg,
    ShaderStage stageArg,
    int16_t countArg = 1>
struct ShaderInterfaceBindingMeta
{
    using BindingType = BindingTypeT;

    enum : int16_t
    {
        type = static_cast<int16_t>(blockTypeArg),
        stage = static_cast<int16_t>(stageArg),
        count = static_cast<int16_t>(countArg)
    };
};

}    //  namespace renderer
