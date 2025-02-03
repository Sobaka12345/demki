#pragma once

#include "handle.hpp"
#include "../utils.hpp"

#include <crtp.hpp>

namespace renderer::vk {
template <typename T>
struct SurfaceFunctions : public CRTPBase<T>
{};

template <typename T>
struct SurfaceGroupFunctions : public CRTPBase<T>
{};

//  to do
//  DECLARE_HANDLE_TYPE(Surface, stub, SurfaceGroupFunctions);
}    //  namespace renderer::vk
