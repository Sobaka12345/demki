#pragma once

#include <graphical_application.hpp>
#include <update_timer.hpp>

class IModel;
class ITexture;
class IComputer;
class Particles;
class Camera;
class Renderable;
struct DeltaTime;

class ParticlesApplication : public GraphicalApplication
{
public:
    ParticlesApplication();
    ~ParticlesApplication();

private:
    virtual void update(int64_t dt) override;
    virtual void perform() override;

private:
    UpdateTimer<TimeResolution> m_timer;

    std::shared_ptr<IComputer> m_computer;
    std::shared_ptr<IRenderer> m_renderer;
    std::shared_ptr<IPipeline> m_computePipeline;
    std::shared_ptr<IPipeline> m_graphicsPipeline;

    std::unique_ptr<DeltaTime> m_deltaTime;
    std::unique_ptr<Particles> m_particles;

    std::shared_ptr<Camera> m_camera;

    std::shared_ptr<IModel> m_model;
    std::shared_ptr<ITexture> m_texture;
    std::shared_ptr<Renderable> m_renderable;
};
