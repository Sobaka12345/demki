#pragma once

#include <memory>

namespace renderer {
class ITexture;
class IMesh;
}

namespace engine {

class Object
{
public:
    virtual ~Object() {}

private:
    std::shared_ptr<renderer::ITexture> m_texture;
    std::shared_ptr<renderer::IMesh> m_mesh;
};

}
