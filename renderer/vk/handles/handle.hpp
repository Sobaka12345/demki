#pragma once
#include <map>
#include <utility>
#include <array>
#include <vector>
#include <set>
#include <tuple>
#include <unordered_map>

template <template <typename> class BaseClass, typename HelperClass, class ContainerT>
struct HandleGroupBase
    : public ContainerT
    , public BaseClass<HelperClass>
{
    using Container = ContainerT;
    using Functions = HelperClass;
    using ElType = HelperClass::Handle;

    template <class... U>
    HandleGroupBase(U&&... u)
        : ContainerT{ std::forward<U>(u)... }
    {}

    void destroyAll(auto... args)
    {
        for (auto it = Container::begin(); it != Container::end(); ++it)
        {
            Functions::destroy(args..., *it, nullptr);
        }
        Container::clear();
    }
};

#define DECLARE_HANDLE_DESTROY_FUNC(TypeName, destroyFunc)                           \
    using TypeName##DestroyArgList = FuncArgTypeList<decltype(destroyFunc)>::Result; \
                                                                                     \
    template <typename... Args>                                                      \
    struct TypeName##DestroyT                                                        \
    {                                                                                \
        inline static auto destroy(Args... args) noexcept                            \
        {                                                                            \
            return destroyFunc(args...);                                             \
        }                                                                            \
    };                                                                               \
    template <>                                                                      \
    struct TypeName##DestroyT<NullType>                                              \
    {};                                                                              \
    using TypeName##Destroy = Apply<TypeName##DestroyT, TypeName##DestroyArgList>::Result;


#define DECLARE_HANDLE_CREATE_FUNC(TypeName, createFunc, multiCreate)                           \
    using TypeName##CreateArgList = FuncArgTypeList<decltype(createFunc)>::Result;              \
    using TypeName##CreateArgListReduced = Pop<TypeName##CreateArgList>::Result;                \
    using TypeName##CreateRetType = FuncReturnType<decltype(createFunc)>::Result;               \
                                                                                                \
    template <typename... Args>                                                                 \
    struct TypeName##CreateT                                                                    \
    {};                                                                                         \
                                                                                                \
    template <>                                                                                 \
    struct TypeName##CreateT<Vk##TypeName>                                                      \
    {};                                                                                         \
                                                                                                \
    template <typename T, typename... Args>                                                     \
    struct TypeName##CreateT<T, Args...>                                                        \
    {                                                                                           \
        [[nodiscard]] inline static T create(Args... args, ::std::string message = {}) noexcept \
        {                                                                                       \
            static_assert(!(multiCreate));                                                      \
            T result;                                                                           \
                                                                                                \
            if constexpr (std::is_integral_v<TypeName##CreateRetType>)                          \
                ASSERT(createFunc(args..., &result) == VK_SUCCESS, message);                    \
            else                                                                                \
                createFunc(args..., &result);                                                   \
            return result;                                                                      \
        }                                                                                       \
                                                                                                \
        inline static VkResult create(Args... args, T* pResult) noexcept                        \
        {                                                                                       \
            return createFunc(args..., pResult);                                                \
        }                                                                                       \
    };                                                                                          \
                                                                                                \
    using TypeName##Create = Apply<TypeName##CreateT,                                           \
        Append<Vk##TypeName, TypeName##CreateArgListReduced>::Result>::Result;


#define DECLARE_HANDLE_TYPE_FULL_IMPL(TypeName, createFunc, destroyFunc, HandleCRTP, GroupCRTP, \
    multiCreate)                                                                                \
    DECLARE_HANDLE_CREATE_FUNC(TypeName, createFunc, multiCreate)                               \
    DECLARE_HANDLE_DESTROY_FUNC(TypeName, destroyFunc)                                          \
                                                                                                \
    struct TypeName##Helper                                                                     \
        : public TypeName##Create                                                               \
        , public TypeName##Destroy                                                              \
        , public HandleCRTP<TypeName##Helper>                                                   \
    {                                                                                           \
        using Handle = Vk##TypeName;                                                            \
        struct Container                                                                        \
        {                                                                                       \
            template <size_t size>                                                              \
            using Array =                                                                       \
                HandleGroupBase<GroupCRTP, TypeName##Helper, ::std::array<Handle, size>>;       \
                                                                                                \
            template <typename Allocator = ::std::allocator<Handle>>                            \
            using Vector =                                                                      \
                HandleGroupBase<GroupCRTP, TypeName##Helper, ::std::vector<Handle, Allocator>>; \
                                                                                                \
            template <typename Comparator = ::std::less<Handle>,                                \
                typename Allocator = ::std::allocator<Handle>>                                  \
            using Set = HandleGroupBase<GroupCRTP,                                              \
                TypeName##Helper,                                                               \
                ::std::set<Handle, Comparator, Allocator>>;                                     \
                                                                                                \
            template <typename Key,                                                             \
                typename Hash = ::std::hash<Key>,                                               \
                typename Pred = ::std::equal_to<Key>,                                           \
                typename Alloc = ::std::allocator<::std::pair<const Key, Handle>>>              \
            using HashMap = HandleGroupBase<GroupCRTP,                                          \
                TypeName##Helper,                                                               \
                ::std::unordered_map<Key, Handle, Hash, Pred, Alloc>>;                          \
                                                                                                \
            template <typename Key,                                                             \
                typename Compare = ::std::less<Key>,                                            \
                typename Alloc = ::std::allocator<::std::pair<const Key, Handle>>>              \
            using Map = HandleGroupBase<GroupCRTP,                                              \
                TypeName##Helper,                                                               \
                ::std::map<Key, Handle, Compare, Alloc>>;                                       \
        };                                                                                      \
                                                                                                \
    private:                                                                                    \
        TypeName##Helper(){};                                                                   \
    };                                                                                          \
                                                                                                \
    using TypeName = TypeName##Helper::Handle;                                                  \
    using TypeName##Container = TypeName##Helper::Container;


#define DECLARE_HANDLE_TYPE(TypeName, HandleCRTP, GroupCRTP)                                \
    DECLARE_HANDLE_TYPE_FULL(TypeName, vkCreate##TypeName, vkDestroy##TypeName, HandleCRTP, \
        GroupCRTP)

#define DECLARE_HANDLE_TYPE_FULL(TypeName, createFunc, destroyFunc, HandleCRTP, GroupCRTP) \
    DECLARE_HANDLE_TYPE_FULL_IMPL(TypeName, createFunc, destroyFunc, HandleCRTP, GroupCRTP, false)
