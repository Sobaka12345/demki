#ifndef GLSL_TYPES_HPP
#define GLSL_TYPES_HPP

// TO DO: THINK ABOUT ANOTHER WAY OF INCLUDING EXTERNAL HEADERS TO GLSL
#ifndef GLSL_HOST
#include "../../utils/glsl_defs.hpp"
#endif

struct alignas(16) DrawCommand
{
    vec4 position;
    vec4 orientation;
    float scale;
};
REGISTER_GLSL_TYPE(DrawCommands)

#endif    //  GLSL_TYPES_HPP
