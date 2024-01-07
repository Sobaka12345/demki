#pragma once

#include <type_traits>
#include <tuple>
#include <variant>

//  template <typename F, typename... Args>
//  struct TypeList
//{ /*
//       using Nodes = TypeList<F, TypeList<Args...>>;*/
//      using Head = F;
//      using Tail = TypeList<Args..., tl::NullType>;
//      using TypesPack = TypesPacker<F, Args...>;
//  };

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
