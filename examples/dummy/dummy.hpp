#pragma once

#include <graphical_application.hpp>
#include <update_timer.hpp>

namespace renderer {
class IMesh;
class ITexture;
class Camera;
class Renderable;
}

class Dummy : public engine::GraphicalApplication
{
public:
    Dummy(int& argc, char** argv);
    ~Dummy();

private:
    virtual void update(int64_t dt) override;
    virtual void perform() override;

private:
    UpdateTimer<TimeResolution> m_timer;

    std::shared_ptr<renderer::IRenderer> m_renderer;
    std::shared_ptr<renderer::IPipeline> m_renderPipeline;

    std::shared_ptr<renderer::Camera> m_camera;

    std::shared_ptr<renderer::IMesh> m_mesh;
    std::shared_ptr<renderer::ITexture> m_texture;
    std::shared_ptr<renderer::Renderable> m_renderable;
};
