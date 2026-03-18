#include "resource.hpp"

namespace resources 
{

template<>
Resource<Vk>::Resource(gapi::GApiContext<Vk>& ctx, std::filesystem::path rootPath)
    : Resource(std::move(rootPath))
{

}

template<>
MeshView Resource<Vk>::mapBundleToGPU(Resource<Vk>::MeshHandle handle) noexcept
{
}


}