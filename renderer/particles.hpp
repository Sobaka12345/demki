#pragma once

#include "uniform.hpp"

class Particles : public SIUniformContainer<Particles>
{
public:
    static constexpr std::array<UniformBinding, 1> s_layout = { UniformBinding{
        .id = UniformID::CAMERA,
        .type = UniformType::DYNAMIC,
        .stage = UniformStage::VERTEX,
    } };
};
