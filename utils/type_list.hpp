#pragma once

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

template <typename TypeList, typename... Types>
struct Append;

template <>
struct Append<NullType, NullType>
{
    using Result = NullType;
};

template <typename... T>
struct Append<NullType, T...>
{
    using Result = TypeList<T...>;
};

template <typename T>
struct Append<T, NullType>
{
    using Result = TypeList<T>;
};

template <typename Head, typename... Tail>
struct Append<NullType, TypeList<Head, Tail...>>
{
    using Result = TypeList<Head, Tail...>;
};

template <typename... Types>
struct Append<TypeList<Types...>, NullType>
{
    using Result = TypeList<Types...>;
};

template <typename Head, typename T, typename... Tail>
struct Append<NullType, TypeList<Head, Tail...>, T>
{
    using Result = TypeList<Head, Tail..., T>;
};

template <typename Head, typename T, typename... Tail>
struct Append<TypeList<Head, Tail...>, T>
{
    using Result = TypeList<Head, Tail..., T>;
};

template <typename Head, typename T, typename... Tail>
struct Append<T, TypeList<Head, Tail...>>
{
    using Result = TypeList<T, Head, Tail...>;
};

template <typename Head1, typename Head2, typename ...Tail1, typename... Tail2>
struct Append<TypeList<Head1, Tail1...>, TypeList<Head2, Tail2...>>
{
    using Result = TypeList<Head1, Tail1..., Head2, Tail2...>;
};

template <typename TypeList, size_t count = 1>
struct Preserve;

template <typename ...Tail>
struct Preserve<TypeList<Tail...>, 0>
{
    using Result = NullType;
};

template <typename HeadList, typename Head, typename ...Tail>
struct Preserve<TypeList<HeadList, Head, Tail...>, 1>
{
    using Result = HeadList;
};

template <typename HeadList, typename Head, size_t count, typename ...Tail>
struct Preserve<TypeList<HeadList, Head, Tail...>, count>
{
    using Result = Preserve<TypeList<typename Append<NullType, HeadList, Head>::Result, Tail...>, count - 1>::Result;
};

template <typename TList, size_t count = 1>
struct Pop
{
    using Result = Preserve<TList, SizeOf<TList>::value - count>::Result;
};

template <template <class...> typename TT, typename TypeListT, typename... TList>
struct Apply;

template <template <class...> typename TT, typename... TList>
struct Apply<TT, NullType, TList...>
{
    using Result = TT<TList...>;
};

template <template <class...> typename TT, typename TypeListT, typename... TList>
struct Apply
{
    using Result = Apply<TT, typename TypeListT::Tail, TList..., typename TypeListT::Head>::Result;
};

template <typename T>
struct FuncArgTypeList;

template <typename FuncT, typename ... Args>
struct FuncArgTypeList<FuncT(Args...)>
{
    using Result = TypeList<Args...>;
};

template <typename T>
struct FuncReturnType;

template <typename FuncT, typename ... Args>
struct FuncReturnType<FuncT(Args...)>
{
    using Result = FuncT;
};

inline void stub(NullType) {}
