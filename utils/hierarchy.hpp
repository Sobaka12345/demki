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
{
    using Unit<T1, LinearHierarchy<typename TypeList<T1, TailTypes...>::Tail, Unit, Root>>::Unit;
};

template <typename T, template <class AtomicType, class Base> typename Unit, typename Root>
struct LinearHierarchy<TypeList<T>, Unit, Root> : Unit<T, Root>
{
    using Unit<T, Root>::Unit;
};

template <typename TypeListT,
    typename Root,
    template <class AtomicType, class Base>
    typename ...UnitTT>
struct ComplexLinearHierarchy;

template <typename T1, typename Root, template <class, class> typename Unit1,
    template <class, class> typename ...UnitTT, typename... TailTypes>
struct ComplexLinearHierarchy<TypeList<T1, TailTypes...>, Root, Unit1, UnitTT...>
    : public Unit1<T1, ComplexLinearHierarchy<typename TypeList<T1, TailTypes...>::Tail, Root, UnitTT...>>
{
    using Super = Unit1<T1, ComplexLinearHierarchy<typename TypeList<T1, TailTypes...>::Tail, Root, UnitTT...>>;
    using Super::Super;
};

template <typename T, typename Root, template <class AtomicType, class Base> typename Unit1>
struct ComplexLinearHierarchy<TypeList<T>, Root, Unit1> : public Unit1<T, Root>
{
    using Super = Unit1<T, Root>;
    using Super::Super;
};

template <typename TypeListT,
    template <typename AtomicType> class Unit,
    typename Root = EmptyType>
struct NonLinearHierarchy;

template <typename T1,  template <class> typename Unit, typename Root, typename... TailTypes>
struct NonLinearHierarchy<TypeList<T1, TailTypes...>, Unit, Root> : Unit<T1>, NonLinearHierarchy<TypeList<TailTypes...>, Unit, Root>
{
    using Super = Unit<T1>;
    using Super::Super;
};

template <typename T, template <class> typename Unit, typename Root>
struct NonLinearHierarchy<TypeList<T>, Unit, Root> : Unit<T>, Root
{
    using Super = Unit<T>;
    using Super::Super;
    using Root::Root;
};

template <typename T, template <class> typename Unit>
struct NonLinearHierarchy<TypeList<T>, Unit, EmptyType> : Unit<T>
{
    using Super = Unit<T>;
    using Super::Super;
};

template <typename TypeListT,
    typename Root = EmptyType>
struct SimpleNonLinearHierarchy;

template <typename T1, typename Root, typename... TailTypes>
struct SimpleNonLinearHierarchy<TypeList<T1, TailTypes...>, Root> : T1, SimpleNonLinearHierarchy<TypeList<TailTypes...>, Root> 
{
    using Super = T1;
    using Super::Super;
};

template <typename T, typename Root>
struct SimpleNonLinearHierarchy<TypeList<T>, Root> :  T, Root
{
    using T::T;
    using Root::Root;
};

template <typename T>
struct SimpleNonLinearHierarchy<TypeList<T>, EmptyType> : T
{
    using T::T;
};
