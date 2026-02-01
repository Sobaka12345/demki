#ifndef GLSL_DEFS_HPP
#define GLSL_DEFS_HPP

#ifndef GLSL_HOST
#	define alignas(T)
#endif

#define STRUCT(T) struct alignas(16) T

#ifdef GLSL_HOST

#	    include <map>

using uint = glm::uint32;
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4x4 = glm::mat4x4;
using IndexType = uint;

//  struct GlslType
//  {
//  private:
//      static std::map<std::string, GlslType> s_typeByString;
//      static std::vector<std::map<std::string, GlslType>::const_iterator> s_typeByIndex;

//  public:
//      static bool addType(std::string name, GlslType type);

//    size_t size = {};
//    size_t alignment = {};
//  };

#	define REGISTER_GLSL_TYPE(                                   \
        Type)  /*                                                 \
                bool Type##Registered = !GlslType::addType(#Type, \
                GlslType{ .size = sizeof(Type), .alignment = std::alignment_of_v<Type> });*/

#else
#	define REGISTER_GLSL_TYPE(Type)
#	define IndexType uint
#endif

STRUCT(Vertex3D)
{
    alignas(16) vec3 pos;

#ifdef GLSL_HOST
    bool operator==(const Vertex3D& other) const
    {
        return pos == other.pos;
    }
#endif
};

STRUCT(Vertex3DColored)
{
    alignas(16) vec3 pos;
    alignas(16) vec3 color;

#ifdef GLSL_HOST
    bool operator==(const Vertex3DColored& other) const
    {
        return pos == other.pos && color == other.color;
    }
#endif
};

STRUCT(Vertex3DColoredTextured)
{
    alignas(16) vec3 pos;
    alignas(16) vec3 color;
    alignas(16) vec2 texture;

#ifdef GLSL_HOST
    bool operator==(const Vertex3DColoredTextured& other) const
    {
        return pos == other.pos && color == other.color && texture == other.texture;
    }
#endif
};

#endif    //  GLSL_DEFS_HPP
