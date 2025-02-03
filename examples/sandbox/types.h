#ifndef TYPES_H
#define TYPES_H

#ifdef GLSL_HOST
#include <cstdint>

typedef uint32_t uint;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
#endif

#define SHADER_STRUCT(NAME)

struct Draw
{
    vec3 position;
    float scale;
    vec4 orientation;
};
#define Draw 6

#endif // TYPES_H
