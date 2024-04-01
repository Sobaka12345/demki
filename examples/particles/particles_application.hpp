#pragma once

#include <graphical_application.hpp>
#include <qt_application.hpp>
#include <update_timer.hpp>

namespace renderer {

class IModel;
class ITexture;
class IComputer;
class Particles;
class Camera;
class Renderable;
}

class DeltaTime;

class ParticlesApplication : public engine::QtApplication
{
public:
    ParticlesApplication(int& argc, char** argv);
    ~ParticlesApplication();

private:
    virtual void update(int64_t dt) override;
    virtual void perform() override;

private:
    std::shared_ptr<renderer::IComputer> m_computer;
    std::shared_ptr<renderer::IRenderer> m_renderer;
    std::shared_ptr<renderer::IPipeline> m_computePipeline;
    std::shared_ptr<renderer::IPipeline> m_graphicsPipeline;

    std::unique_ptr<DeltaTime> m_deltaTime;
    std::unique_ptr<renderer::Particles> m_particles;

    std::shared_ptr<renderer::Camera> m_camera;

    std::shared_ptr<renderer::IModel> m_model;
    std::shared_ptr<renderer::ITexture> m_texture;
    std::shared_ptr<renderer::Renderable> m_renderable;
};
