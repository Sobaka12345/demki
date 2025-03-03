#pragma once
#include <map>
#include <utility>
#include <array>
#include <vector>
#include <set>
#include <tuple>
#include <unordered_map>

template <template <typename> class BaseClass, typename FunctionsT, class ContainerT>
struct HandleGroupBase
    : public ContainerT
    , public BaseClass<FunctionsT>
{
    using Container = ContainerT;
    using Functions = FunctionsT;
    using ElType = FunctionsT::Handle;

    template <class... U>
    HandleGroupBase(U&&... u)
        : ContainerT{ std::forward<U>(u)... }
    {}

    void destroyAll(auto... args)
    {
        for (auto it = Container::begin(); it != Container::end(); ++it)
        {
            if constexpr (requires(Container::value_type& t) { t.second; })
            {
                Functions::destroy(args..., it->second, nullptr);
            } else {
                Functions::destroy(args..., *it, nullptr);
            }
        }

        if constexpr (requires(Container& t) { t.clear(); })
        {
            Container::clear();
        }
    }
};

#define DECLARE_HANDLE_TYPE(TypeName, HandleCRTP, GroupCRTP)                                       \
    struct TypeName : public HandleCRTP<TypeName>                                                  \
    {                                                                                              \
        TypeName() = delete;                                                                       \
        using Handle = Vk##TypeName;                                                               \
                                                                                                   \
        template <size_t size>                                                                     \
        using Array = HandleGroupBase<GroupCRTP, TypeName, ::std::array<Vk##TypeName, size>>;      \
                                                                                                   \
        template <typename Allocator = ::std::allocator<Vk##TypeName>>                             \
        using Vector =                                                                             \
            HandleGroupBase<GroupCRTP, TypeName, ::std::vector<Vk##TypeName, Allocator>>;          \
                                                                                                   \
        template <typename Comparator = ::std::less<Vk##TypeName>,                                 \
            typename Allocator = ::std::allocator<Vk##TypeName>>                                   \
        using Set =                                                                                \
            HandleGroupBase<GroupCRTP, TypeName, ::std::set<Vk##TypeName, Comparator, Allocator>>; \
                                                                                                   \
        template <typename Key,                                                                    \
            typename Hash = ::std::hash<Key>,                                                      \
            typename Pred = ::std::equal_to<Key>,                                                  \
            typename Alloc = ::std::allocator<::std::pair<const Key, Vk##TypeName>>>               \
        using HashMap = HandleGroupBase<GroupCRTP,                                                 \
            TypeName,                                                                              \
            ::std::unordered_map<Key, Vk##TypeName, Hash, Pred, Alloc>>;                           \
                                                                                                   \
        template <typename Key,                                                                    \
            typename Compare = ::std::less<Key>,                                                   \
            typename Alloc = ::std::allocator<::std::pair<const Key, Vk##TypeName>>>               \
        using Map =                                                                                \
            HandleGroupBase<GroupCRTP, TypeName, ::std::map<Key, Vk##TypeName, Compare, Alloc>>;   \
    };

#define FE_1(WHAT, X)      WHAT(X)
#define FE_2(WHAT, X, ...) WHAT(X) FE_1(WHAT, __VA_ARGS__)
#define FE_3(WHAT, X, ...) WHAT(X) FE_2(WHAT, __VA_ARGS__)
#define FE_4(WHAT, X, ...) WHAT(X) FE_3(WHAT, __VA_ARGS__)
#define FE_5(WHAT, X, ...) WHAT(X) FE_4(WHAT, __VA_ARGS__)

#define GET_MACRO(_1, _2, _3, _4, _5, NAME, ...) NAME
#define FOR_EACH(action, ...) \
    __VA_OPT__(GET_MACRO(__VA_ARGS__, FE_5, FE_4, FE_3, FE_2, FE_1)(action, __VA_ARGS__))

#define VK_TYPE_VAR(Type)     Type,
#define VK_TYPE_VAR_DEF(Type) Vk##Type Type,

#define CREATE_FUNC_FULL(Type, CreateInfoType, CreateFunc, ...)                                   \
    [[nodiscard]] static constexpr inline Vk##Type create(                                        \
        FOR_EACH(VK_TYPE_VAR_DEF, __VA_ARGS__) const CreateInfoType* pCreateInfo,                 \
        const VkAllocationCallbacks* pAllocator, std::string message = {}) noexcept               \
    {                                                                                             \
        Vk##Type result = VK_NULL_HANDLE;                                                         \
        ASSERT(CreateFunc(FOR_EACH(VK_TYPE_VAR, __VA_ARGS__) pCreateInfo, pAllocator, &result) == \
                VK_SUCCESS,                                                                       \
            message);                                                                             \
        return result;                                                                            \
    }

#define CREATE_FUNC(Type, ...) \
    CREATE_FUNC_FULL(Type, Vk##Type##CreateInfo, vkCreate##Type, __VA_ARGS__)

#define DESTROY_FUNC_FULL(Type, DestroyFunc, ...)                                                \
    static constexpr inline void destroy(FOR_EACH(VK_TYPE_VAR_DEF, __VA_ARGS__) Vk##Type handle, \
        const VkAllocationCallbacks* pAllocator) noexcept                                        \
    {                                                                                            \
        DestroyFunc(FOR_EACH(VK_TYPE_VAR, __VA_ARGS__) handle, pAllocator);                      \
    }

#define DESTROY_FUNC(Type, ...) DESTROY_FUNC_FULL(Type, vkDestroy##Type, __VA_ARGS__)
