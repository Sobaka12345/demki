#pragma once

#include <assert.hpp>
#include <types.hpp>

#include <vulkan/vulkan.h>

#include <type_traits>
#include <span>

namespace std {
template <typename Type>
struct is_std_span : std::false_type
{};

template <typename Item, std::size_t N>
struct is_std_span<std::span<Item, N>> : std::true_type
{};
}

namespace renderer::vk {

inline VkSampleCountFlagBits toVkSampleFlagBits(Multisampling sampleCount)
{
    switch (sampleCount)
    {
        case Multisampling::MSA_64X: return VK_SAMPLE_COUNT_64_BIT;
        case Multisampling::MSA_32X: return VK_SAMPLE_COUNT_32_BIT;
        case Multisampling::MSA_16X: return VK_SAMPLE_COUNT_16_BIT;
        case Multisampling::MSA_8X: return VK_SAMPLE_COUNT_8_BIT;
        case Multisampling::MSA_4X: return VK_SAMPLE_COUNT_4_BIT;
        case Multisampling::MSA_2X: return VK_SAMPLE_COUNT_2_BIT;
        case Multisampling::MSA_1X: return VK_SAMPLE_COUNT_1_BIT;
    }

    ASSERT(false, "sample count not declared");
    return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
}

template <typename T, VkStructureType sTypeArg>
struct VkStruct : public T
{
    using VkStructType = T;

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

template <typename T>
concept IsVkStruct = requires {
    {
        typename T::VkStructType{}
    } -> std::convertible_to<T>;
};

}    //  namespace renderer::vk

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

#define VKSTRUCT_PROPERTY(_type, name)                                                            \
                                                                                                  \
private:                                                                                          \
    template <typename T>                                                                         \
    constexpr inline void _set##name(T value)                                                     \
    {                                                                                             \
        if constexpr (std::is_std_span<T>::value)                                                 \
            std::copy(value.begin(), value.end(), Base::name);                                    \
        else                                                                                      \
            Base::name = value;                                                                   \
    }                                                                                             \
                                                                                                  \
public:                                                                                           \
    template <typename T = _type>                                                                 \
    constexpr                                                                                     \
        typename std::enable_if<!std::is_std_span<T>::value, const std::remove_const_t<T>&>::type \
        name() const                                                                              \
    {                                                                                             \
        if constexpr (IsVkStruct<T>)                                                              \
        {                                                                                         \
            return reinterpret_cast<const T&>(this->*(&Base::name));                              \
        }                                                                                         \
        else                                                                                      \
            return Base::name;                                                                    \
    }                                                                                             \
                                                                                                  \
    template <typename T = _type>                                                                 \
    constexpr typename std::enable_if<std::is_std_span<T>::value,                                 \
        std::span<const std::remove_const_t<typename T::value_type>, T::extent>>::type            \
        name() const                                                                              \
    {                                                                                             \
        return { Base::name };                                                                    \
    }                                                                                             \
                                                                                                  \
    template <typename T = _type>                                                                 \
    constexpr typename std::enable_if<!std::is_std_span<T>::value, T&>::type name()               \
    {                                                                                             \
        if constexpr (IsVkStruct<T>)                                                              \
        {                                                                                         \
            return reinterpret_cast<T&>(this->*(&Base::name));                                    \
        }                                                                                         \
        else                                                                                      \
            return Base::name;                                                                    \
    }                                                                                             \
                                                                                                  \
    template <typename T = _type>                                                                 \
    constexpr typename std::enable_if<std::is_std_span<T>::value,                                 \
        std::span<std::remove_const_t<typename T::value_type>, T::extent>>::type                  \
        name()                                                                                    \
    {                                                                                             \
        return { Base::name };                                                                    \
    }                                                                                             \
                                                                                                  \
    template <typename T = _type>                                                                 \
    constexpr auto& name(T&& value)                                                               \
    {                                                                                             \
        _set##name<_type>(std::forward<T>(value));                                                \
        return *this;                                                                             \
    }
