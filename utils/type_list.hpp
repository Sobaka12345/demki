#pragma once

#include <type_traits>
#include <cstdlib>

struct EmptyType
{};

struct NullType
{};

template <typename HeadT, typename... TailTT>
struct TypeList;

template <bool cond, typename... Args>
struct TypeListTail
{
    using type = NullType;
};

template <typename... Args>
struct TypeListTail<false, Args...>
{
    using type = NullType;
};

template <typename... Args>
struct TypeListTail<true, Args...>
{
    using type = TypeList<Args...>;
};

template <typename HeadT, typename... TailTT>
struct TypeList
{
    using Head = HeadT;
    using Tail = TypeListTail<sizeof...(TailTT) != 0U, TailTT...>::type;
};

template <typename TypeListT>
struct SizeOf
{
    enum
    {
        value = SizeOf<typename TypeListT::Tail>::value + 1,
    };
};

template <>
struct SizeOf<NullType>
{
    enum
    {
        value = 0,
    };
};

template <typename TypeListT, typename T>
struct IndexOf;

template <typename T>
struct IndexOf<NullType, T>
{
    enum
    {
        value = -1,
    };
};

template <typename T, typename... Args>
struct IndexOf<TypeList<T, Args...>, T>
{
    enum
    {
        value = 0,
    };
};

template <typename TypeListT, typename T>
struct IndexOf
{
private:
    enum
    {
        temp = IndexOf < typename TypeListT::Tail,
        T > ::value,
    };

public:
    enum
    {
        value = temp == -1 ? -1 : temp + 1,
    };
};

template <typename TypeListT, size_t index>
struct TypeAt;

template <typename TypeListT>
struct TypeAt<TypeListT, 0U>
{
    using type = TypeListT::Head;
};

template <typename TypeListT, size_t index>
struct TypeAt
{
    using type = TypeAt<typename TypeListT::Tail, index - 1>::type;
};
