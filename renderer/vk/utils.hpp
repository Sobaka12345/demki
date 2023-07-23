#pragma once

#include <vulkan/vulkan.h>

#include <array>

namespace std {
template <typename Type>
struct is_std_array : std::false_type
{};

template <typename Item, std::size_t N>
struct is_std_array<std::array<Item, N>> : std::true_type
{};
}

namespace vk {

template <typename T, VkStructureType sTypeArg>
struct VkStruct : public T
{
    constexpr VkStruct(T structValue) noexcept
        : T{ structValue }
    {
        if constexpr (sTypeArg != VK_STRUCTURE_TYPE_MAX_ENUM)
        {
            T::sType = sTypeArg;
        }

        static_assert(sizeof(VkStruct) == sizeof(T));
    }
};

}    //  namespace vk

#define COMMA ,

#define BEGIN_DECLARE_VKSTRUCT_IMPL(structName, vkStructName, sTypeVal) \
    struct structName : public VkStruct<vkStructName, sTypeVal>         \
    {                                                                   \
    private:                                                            \
        using Base = vkStructName;                                      \
                                                                        \
    public:                                                             \
        constexpr structName() noexcept                                 \
            : VkStruct<vkStructName, sTypeVal>(vkStructName{})          \
        {}                                                              \
                                                                        \
        constexpr structName(vkStructName sValue) noexcept              \
            : VkStruct<vkStructName, sTypeVal>(std::move(sValue))       \
        {                                                               \
            static_assert(sizeof(structName) == sizeof(vkStructName));  \
        }

#define END_DECLARE_VKSTRUCT() \
    }                          \
    ;

#define BEGIN_DECLARE_VKSTRUCT(structName, sTypeVal) \
    BEGIN_DECLARE_VKSTRUCT_IMPL(structName, Vk##structName, sTypeVal)

#define BEGIN_DECLARE_UNTYPED_VKSTRUCT(structName) \
    BEGIN_DECLARE_VKSTRUCT(structName, VK_STRUCTURE_TYPE_MAX_ENUM)

#define VKSTRUCT_PROPERTY(type, name)                          \
                                                               \
private:                                                       \
    template <typename T>                                      \
    constexpr inline void _set##name(T value)                  \
    {                                                          \
        if constexpr (std::is_std_array<T>::value)             \
            std::copy(value.begin(), value.end(), Base::name); \
        else                                                   \
            Base::name = value;                                \
    }                                                          \
                                                               \
public:                                                        \
    template <typename T = type>                               \
    constexpr T name() const                                   \
    {                                                          \
        if constexpr (std::is_std_array<T>::value)             \
        {                                                      \
            return std::to_array<T::value_type>(Base::name);   \
        }                                                      \
        else                                                   \
            return Base::name;                                 \
    }                                                          \
                                                               \
    constexpr auto& name(type value)                           \
    {                                                          \
        _set##name(std::forward<type>(value));                 \
        return *this;                                          \
    }
