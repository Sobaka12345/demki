#pragma once

#include <type_traits>
#include <typeinfo>
#include <typeindex>
#include <tuple>
#include <variant>
#include <boost/pfr.hpp>

#define CREATE_INFO_PROPERTY(type, name, def) \
                                              \
public:                                       \
    CreateInfo& name(type value)              \
    {                                         \
        m_##name = value;                     \
        return *this;                         \
    }                                         \
    const auto& name() const                  \
    {                                         \
        return m_##name;                      \
    }                                         \
    auto& name()                              \
    {                                         \
        return m_##name;                      \
    }                                         \
                                              \
private:                                      \
    type m_##name = def;

template <typename T, typename... Ts>
struct Unique : std::type_identity<T>
{};

template <typename... Ts, typename U, typename... Us>
struct Unique<std::tuple<Ts...>, U, Us...>
    : std::conditional_t<(std::is_same_v<U, Ts> || ...),
          Unique<std::tuple<Ts...>, Us...>,
          Unique<std::tuple<Ts..., U>, Us...>>
{};

template <typename... Ts>
using UniqueTuple = typename Unique<std::tuple<>, Ts...>::type;

template <typename... Ts>
using UniqueVariant = typename Unique<std::variant<>, Ts...>::type;

struct StructMetaInfo
{
    struct Field
    {
        ptrdiff_t shift = 0;
        size_t typeSize = 0;
        std::type_index typeId;
    };

    template <typename T>
        requires std::is_trivial_v<T> && std::is_standard_layout_v<T>
    static StructMetaInfo fromType()
    {
        StructMetaInfo result;
        result.typeSize = sizeof(T);
        result.alignment = std::alignment_of_v<T>;

        T value;
        boost::pfr::for_each_field(value, [&result, &value](auto& subValue) {
            result.fields.push_back(StructMetaInfo::Field{
                .shift = reinterpret_cast<uint8_t*>(&subValue) - reinterpret_cast<uint8_t*>(&value),
                .typeSize = sizeof(decltype(subValue)),
                .typeId = std::type_index{ typeid(decltype(subValue)) } });
        });

        return result;
    }

    size_t alignment = 0;
    size_t typeSize = 0;
    std::vector<Field> fields;
};
