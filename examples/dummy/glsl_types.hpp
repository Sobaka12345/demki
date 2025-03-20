#ifndef GLSL_TYPES_HPP
#define GLSL_TYPES_HPP

// TO DO: THINK ABOUT ANOTHER WAY OF INCLUDING EXTERNAL HEADERS TO GLSL
#ifndef GLSL_HOST
#include "../../utils/glsl_defs.hpp"
#endif

struct Draw
{
    vec3 position;
    float scale;
    vec4 orientation;
};
REGISTER_GLSL_TYPE(Draw)

#endif    //  GLSL_TYPES_HPP
