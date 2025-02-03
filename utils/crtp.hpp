#pragma once

template <typename Derived>
struct CRTPBase
{
protected:
    [[nodiscard]] inline Derived& that() noexcept { return *static_cast<Derived*>(this); }
};
