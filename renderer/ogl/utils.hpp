#pragma once

#include <assert.hpp>

#include <glad/glad.h>

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <map>
#include <tuple>
#include <cstdlib>
#include <typeindex>

namespace glm {

#define IS_GLM_VEC(dim)                                       \
    template <typename T>                                     \
    struct is_glm_vec##dim : std::false_type                  \
    {};                                                       \
                                                              \
    template <typename T>                                     \
    struct is_glm_vec##dim<glm::vec<dim, T>> : std::true_type \
    {};

IS_GLM_VEC(2);
IS_GLM_VEC(3);
IS_GLM_VEC(4);

}    //  namespace glm

#define COMMA ,

#define TYPE_INDEX(T) (std::type_index{ typeid(T) })

#define EACH_GL_TYPE_LIST                                                                         \
    TYPE_INDEX(glm::float32_t), TYPE_INDEX(glm::float64_t), TYPE_INDEX(char), TYPE_INDEX(int8_t), \
        TYPE_INDEX(uint8_t), TYPE_INDEX(int16_t), TYPE_INDEX(uint16_t), TYPE_INDEX(int32_t),      \
        TYPE_INDEX(uint32_t),

#define EACH_VEC_LIST(dim)                                                                    \
    TYPE_INDEX(glm::vec<dim COMMA glm::float32_t>),                                           \
        TYPE_INDEX(glm::vec<dim COMMA glm::float64_t>), TYPE_INDEX(glm::vec<dim COMMA char>), \
        TYPE_INDEX(glm::vec<dim COMMA int8_t>), TYPE_INDEX(glm::vec<dim COMMA uint8_t>),      \
        TYPE_INDEX(glm::vec<dim COMMA int16_t>), TYPE_INDEX(glm::vec<dim COMMA uint16_t>),    \
        TYPE_INDEX(glm::vec<dim COMMA int32_t>), TYPE_INDEX(glm::vec<dim COMMA uint32_t>),

#define EACH_VEC_PAIR(dim)                                                              \
    { TYPE_INDEX(glm::vec<dim COMMA glm::float32_t>), TYPE_INDEX(glm::float32_t) },     \
        { TYPE_INDEX(glm::vec<dim COMMA glm::float64_t>), TYPE_INDEX(glm::float64_t) }, \
        { TYPE_INDEX(glm::vec<dim COMMA char>), TYPE_INDEX(char) },                     \
        { TYPE_INDEX(glm::vec<dim COMMA int8_t>), TYPE_INDEX(int8_t) },                 \
        { TYPE_INDEX(glm::vec<dim COMMA uint8_t>), TYPE_INDEX(uint8_t) },               \
        { TYPE_INDEX(glm::vec<dim COMMA int16_t>), TYPE_INDEX(int16_t) },               \
        { TYPE_INDEX(glm::vec<dim COMMA uint16_t>), TYPE_INDEX(uint16_t) },             \
        { TYPE_INDEX(glm::vec<dim COMMA int32_t>), TYPE_INDEX(int32_t) },               \
        { TYPE_INDEX(glm::vec<dim COMMA uint32_t>), TYPE_INDEX(uint32_t) },


#define EACH_DIM_VEC_LIST \
    EACH_VEC_LIST(2)      \
    EACH_VEC_LIST(3)      \
    EACH_VEC_LIST(4)

#define EACH_DIM_VEC_PAIR \
    EACH_VEC_PAIR(2)      \
    EACH_VEC_PAIR(3)      \
    EACH_VEC_PAIR(4)

namespace ogl {

template <typename T>
inline constexpr GLenum toGLType()
{
    if constexpr (std::is_same_v<glm::float32_t, T>) return GL_FLOAT;
    else if constexpr (std::is_same_v<char, T>)
        return GL_BYTE;
    else if constexpr (std::is_same_v<int8_t, T>)
        return GL_BYTE;
    else if constexpr (std::is_same_v<uint8_t, T>)
        return GL_UNSIGNED_BYTE;
    else if constexpr (std::is_same_v<int16_t, T>)
        return GL_SHORT;
    else if constexpr (std::is_same_v<uint16_t, T>)
        return GL_UNSIGNED_SHORT;
    else if constexpr (std::is_same_v<int32_t, T>)
        return GL_INT;
    else if constexpr (std::is_same_v<uint32_t, T>)
        return GL_UNSIGNED_INT;
    else if constexpr (std::is_same_v<glm::float64_t, T>)
        return GL_DOUBLE;
    else
        []<bool flag = false>()
        {
            static_assert(flag, "no match");
        }
    ();
}

inline GLenum toGLType(std::type_index typeIndex)
{
    if (TYPE_INDEX(glm::float32_t) == typeIndex) return GL_FLOAT;
    else if (TYPE_INDEX(char) == typeIndex)
        return GL_BYTE;
    else if (TYPE_INDEX(int8_t) == typeIndex)
        return GL_BYTE;
    else if (TYPE_INDEX(uint8_t) == typeIndex)
        return GL_UNSIGNED_BYTE;
    else if (TYPE_INDEX(int16_t) == typeIndex)
        return GL_SHORT;
    else if (TYPE_INDEX(uint16_t) == typeIndex)
        return GL_UNSIGNED_SHORT;
    else if (TYPE_INDEX(int32_t) == typeIndex)
        return GL_INT;
    else if (TYPE_INDEX(uint32_t) == typeIndex)
        return GL_UNSIGNED_INT;
    else if (TYPE_INDEX(glm::float64_t) == typeIndex)
        return GL_DOUBLE;

    ASSERT(false, "not implemented");
    return GL_INVALID_VALUE;
}

template <typename Type>
inline constexpr std::tuple<size_t, GLenum> toGLCompoundTypeSize()
{
    if constexpr (glm::is_glm_vec2<Type>::value) return { 2, toGLType<Type::value_type>() };
    else if constexpr (glm::is_glm_vec3<Type>::value)
        return { 3, toGLType<Type::value_type>() };
    else if constexpr (glm::is_glm_vec4<Type>::value)
        return { 4, toGLType<Type::value_type>() };

    return { 1, toGLType<Type>() };
}

inline std::tuple<size_t, GLenum> toGLCompoundTypeSize(std::type_index typeIndex)
{
    static const std::map<std::type_index, std::type_index> s_vec2TypesMap = { EACH_VEC_PAIR(2) };
    static const std::map<std::type_index, std::type_index> s_vec3TypesMap = { EACH_VEC_PAIR(3) };
    static const std::map<std::type_index, std::type_index> s_vec4TypesMap = { EACH_VEC_PAIR(4) };

    if (auto res = s_vec2TypesMap.find(typeIndex); res != s_vec2TypesMap.end())
    {
        return { 2, toGLType(res->second) };
    }
    else if (auto res = s_vec3TypesMap.find(typeIndex); res != s_vec3TypesMap.end())
    {
        return { 3, toGLType(res->second) };
    }
    else if (auto res = s_vec4TypesMap.find(typeIndex); res != s_vec4TypesMap.end())
    {
        return { 4, toGLType(res->second) };
    }

    return { 1, toGLType(typeIndex) };
}

}    //  namespace ogl
