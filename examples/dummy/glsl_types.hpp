#ifndef GLSL_TYPES_HPP
#define GLSL_TYPES_HPP

#ifdef GLSL_HOST
#	include <glsl_defs.hpp>
#else
#	define REGISTER_GLSL_TYPE(Type)
#endif

struct Draw
{
    vec3 position;
    float scale;
    vec4 orientation;
};
REGISTER_GLSL_TYPE(Draw)

#endif    //  GLSL_TYPES_HPP
