#pragma once

#include "type_list.hpp"

template <typename TypeListT,
    template <class AtomicType, class Base>
    typename Unit,
    typename Root = EmptyType>
struct LinearHierarchy;

template <typename T1, template <class, class> typename Unit, typename Root, typename... TailTypes>
struct LinearHierarchy<TypeList<T1, TailTypes...>, Unit, Root>
    : public Unit<T1, LinearHierarchy<typename TypeList<T1, TailTypes...>::Tail, Unit, Root>>
{};

template <typename T, template <class AtomicType, class Base> typename Unit, typename Root>
struct LinearHierarchy<TypeList<T>, Unit, Root> : public Unit<T, Root>
{};
