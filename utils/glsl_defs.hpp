#ifndef GLSL_DEFS_HPP
#define GLSL_DEFS_HPP

#include <map>

using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4x4 = glm::mat4x4;

struct GlslType
{
private:
    static std::map<std::string, GlslType> s_typeByString;
    static std::vector<std::map<std::string, GlslType>::const_iterator> s_typeByIndex;

public:
    static bool addType(std::string name, GlslType type);

    size_t size = {};
    size_t alignment = {};
};

#define REGISTER_GLSL_TYPE(Type)                      \
    bool Type##Registered = !GlslType::addType(#Type, \
        GlslType{ .size = sizeof(Type), .alignment = std::alignment_of_v<Type> });

#endif    //  GLSL_DEFS_HPP
